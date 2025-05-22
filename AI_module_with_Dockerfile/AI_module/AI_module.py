import grpc
import os
import sqlite3
import re
import json
import subprocess
import sync_pb2
import sync_pb2_grpc
import langchain_pb2
import langchain_pb2_grpc
import tag_recommender_pb2
import tag_recommender_pb2_grpc
import logging
import numpy as np

from langchain_chroma import Chroma
from langchain.chat_models import init_chat_model
from langchain.embeddings import init_embeddings
from langchain_core.documents import Document
from langchain_text_splitters import RecursiveCharacterTextSplitter
from langchain_core.tools import tool
from langchain_core.messages import SystemMessage
from langgraph.graph import MessagesState, StateGraph, END
from langgraph.prebuilt import ToolNode, tools_condition
from langgraph.checkpoint.sqlite import SqliteSaver
from top2vec import Top2Vec
from sklearn.metrics.pairwise import cosine_similarity
from concurrent import futures


logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s [%(levelname)s] %(name)s: %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S',
    handlers=[logging.StreamHandler()]
)


class SyncService(sync_pb2_grpc.SyncServiceServicer):

    def __init__(self, config):
        super().__init__()
        self.logger = logging.getLogger(self.__class__.__name__)
        self.embeddings_model = init_embeddings(config['embeddings_model'])
        self.vector_store = Chroma(
            collection_name='note_embeddings',
            embedding_function=self.embeddings_model,
            persist_directory="./note_embeddings",
        )
        self.tag_recommender = None

        if os.path.isfile('tag_recommender'):
            self.tag_recommender = Top2Vec.load('tag_recommender')


    def AddNote(self, request, context):
        note_id = request.note_id
        note_text = request.full_text
        metadata = {}
        for field in note_text.split('\n')[:2]:
            field_name, content = field.split('\t')
            metadata[field_name[:-1]] = content
        note = Document(page_content=note_text, metadata=metadata)
        try:
            self.vector_store.add_documents(documents=[note], ids=[str(note_id)], metadata=[metadata])
            #self.logger.info(f"Existing notes\' ids: {self.vector_store.get()['ids']}")
        except Exception as note_creation_exception:
            context.abort(grpc.StatusCode.INTERNAL,
                          f'Can not add note embedding to vector store: {note_creation_exception}'
                          )

        try:
            notes = self.vector_store.get()['documents']
            self.tag_recommender = Top2Vec(documents=notes,
                                           min_count=1,
                                           embedding_model='doc2vec',
                                           keep_documents=False
                                           )
            self.tag_recommender.save('tag_recommender')
        except Exception as tag_recommender_learning_exception:
            pass

        return sync_pb2.EmptyResponse()

    def UpdateNote(self, request, context):
        note_id = request.note_id
        note_text = request.full_text
        metadata = {}
        for field in note_text.split('\n')[:2]:
            field_name, content = field.split('\t')
            metadata[field_name[:-1]] = content
        note = Document(page_content=note_text, metadata=metadata)
        try:
            self.vector_store.update_document(str(note_id), note)
        except Exception as note_update_exception:
            context.abort(grpc.StatusCode.INTERNAL,
                          f'Can not update note embedding in vector store: {note_update_exception}'
                          )

        try:
            notes = self.vector_store.get()['documents']
            self.tag_recommender = Top2Vec(documents=notes,
                                           min_count=1,
                                           embedding_model='doc2vec',
                                           keep_documents=False
                                           )
            self.tag_recommender.save('tag_recommender')
        except Exception as tag_recommender_learning_exception:
            pass

        return sync_pb2.EmptyResponse()

    def DeleteNote(self, request, context):
        note_id = request.note_id
        try:
            self.vector_store.delete([str(note_id)])
        except Exception as note_deletion_exception:
            context.abort(grpc.StatusCode.INTERNAL,
                          f'Can not delete note embedding from vector store: {note_deletion_exception}'
                          )

        return sync_pb2.EmptyResponse()

class LangChainService(langchain_pb2_grpc.LangChainServicer):

    def __init__(self, config):
        super().__init__()
        self.graph_builder = StateGraph(MessagesState)
        self.chat_model_api_key_name = config['chat_model_api_key']['key_name']
        self.chat_model_api_key = config['chat_model_api_key']['key']
        self.embeddings_model_api_key_name = config['embeddings_model_api_key']['key_name']
        self.embeddings_model_api_key = config['embeddings_model_api_key']['key']
        self.is_chat_model_installed = False
        self.is_embeddings_model_installed = False
        if self.chat_model_api_key_name is None and self.chat_model_api_key is None:
            ollama_model_list = subprocess.run(['ollama', 'list'], capture_output=True, text=True).stdout.split('\n')[1:-1]
            for model_info in ollama_model_list[1:-1]:
                model_name = model_info.split('    ')[0]
                self.is_chat_model_installed = model_name in config['chat_model']
                if not self.is_chat_model_installed:
                    subprocess.run(["ollama", "rm", model_name], capture_output=False)
            if not self.is_chat_model_installed:
                subprocess.run(['ollama', 'pull', config['chat_model'][7:]], capture_output=False)
        else:
            if not os.environ.get(self.chat_model_api_key_name):
                os.environ[self.chat_model_api_key_name] = self.chat_model_api_key
        if self.embeddings_model_api_key_name is None and self.embeddings_model_api_key is None:
            ollama_model_list = subprocess.run(['ollama', 'list'], capture_output=True, text=True).stdout.split('\n')[1:-1]
            for model_info in ollama_model_list[1:-1]:
                model_name = model_info.split('    ')[0]
                self.is_embeddings_model_installed = model_name in config['embeddings_model']
                if not self.is_embeddings_model_installed:
                    subprocess.run(["ollama", "rm", model_name], capture_output=False)
            if not self.is_embeddings_model_installed:
                subprocess.run(['ollama', 'pull', config['embeddings_model'][7:]], capture_output=False)
        else:
            if not os.environ.get(self.embeddings_model_api_key_name):
                os.environ[self.embeddings_model_api_key_name] = self.embeddings_model_api_key
        self.chat_model = init_chat_model(config['chat_model'])
        self.embeddings_model = init_embeddings(config['embeddings_model'])
        self.vector_store = Chroma(
            collection_name='note_embeddings',
            embedding_function=self.embeddings_model,
            persist_directory="./note_embeddings",
        )
        self.n_relevant_notes = config['n_relevant_notes']
        self.n_prompt_recommendations = config['n_prompt_recommendations']
        self.memory = SqliteSaver(sqlite3.connect('checkpoints.sqlite', check_same_thread=False))
        self.chat_id = {"configurable": {"thread_id": "0"}}
        self.initialize_workflow()

    @tool(description='Retrieves docs which are most similar to query.', response_format="content_and_artifact")
    def retrieve(self, query: str):
        retrieved_docs = self.vector_store.similarity_search(query, k=self.n_relevant_notes)
        serialized = "\n".join(f"Content: {doc.page_content}" for doc in retrieved_docs)
        return serialized, retrieved_docs

    def query_or_respond(self, state: MessagesState):
        chat_model_with_tools = self.chat_model.bind_tools([self.retrieve])
        response = chat_model_with_tools.invoke(state["messages"])
        return {"messages": [response]}

    def generate(self, state: MessagesState):
        recent_tool_messages = []
        for message in reversed(state["messages"]):
            if message.type == "tool":
                recent_tool_messages.append(message)
            else:
                break
        tool_messages = recent_tool_messages[::-1]

        docs_content = "\n\n".join(doc.content for doc in tool_messages)
        system_message_content = (
            "You are an assistant for question-answering tasks.\n"
            "STRICT RULES:\n"
            "1. Answer **ONLY** using the provided context below\n"
            "2. If the context is irrelevant/empty or you're uncertain, say 'I don't know'\n"
            "3. Never mention tools, functions, or internal systems\n"
            "4. Never invent names/dates/numbers not explicitly in the context\n"
            "\n\n"
            f"{docs_content}"
        )
        conversation_messages = [
            message
            for message in state["messages"]
            if message.type in ("human", "system")
               or (message.type == "ai" and not message.tool_calls)
        ]
        prompt = [SystemMessage(system_message_content)] + conversation_messages

        response = self.chat_model.invoke(prompt)
        return {"messages": [response]}

    def initialize_workflow(self):
        tools = ToolNode([self.retrieve])
        self.graph_builder.add_node(self.query_or_respond)
        self.graph_builder.add_node(tools)
        self.graph_builder.add_node(self.generate)

        self.graph_builder.set_entry_point("query_or_respond")
        self.graph_builder.add_conditional_edges(
            "query_or_respond",
            tools_condition,
            {END: END, "tools": "tools"},
        )
        self.graph_builder.add_edge("tools", "generate")
        self.graph_builder.add_edge("generate", END)

        self.workflow = self.graph_builder.compile(checkpointer=self.memory)

    def GenerateAnswer(self, request, context):
        query = request.query_text
        query_answer = (self.workflow.invoke({"messages": [{"role": "user", "content": query}]},
                                             stream_mode='values',
                                             config=self.chat_id
                                             )['messages'][-1].content
                        )
        return langchain_pb2.ChatbotAnswer(**{'answer_text': query_answer})

    def SemanticSearch(self, request, context):
        query = request.query_text
        retrieved_docs = self.vector_store.search(query, k=self.n_relevant_notes, search_type='similarity')
        retrieved_docs_ids = []
        for doc in retrieved_docs:
            retrieved_docs_ids.append(int(doc.id))
        return langchain_pb2.SemanticSearchResult(**{'found_documents_ids': retrieved_docs_ids})

    def RecommendPrompts(self, request, context):
        input_message = f'''###Instructions:
        1. Analyze the user's past queries to understand their interests and intent.
        2. Generate exactly {self.n_prompt_recommendations} new, relevant queries that the user is likely to be interested in.
        
        ### Response Format Requirement
        Generate exactly {self.n_prompt_recommendations} recommendations as a JSON array of plain text strings using this EXACT format:
        ["Query 1", "Query 2"]
        
        ### Absolute Rules:
        1. NO dictionaries, nested objects, or key-value pairs
        2. NO function calls (retrieve/search/any(...)), symbols (=, <, >), or code
        3. NO text outside the array (no explanations, prefixes, or suffixes)
        4. Each item must be a complete question/suggestion (10-25 words)
        5. The recommendations should be diverse but related to the user's past queries.
        
        ### Valid Examples:
        ["How to create a project plan for software development?",
        "Best practices for task prioritization in agile teams"]
        
        ### Invalid Examples:
        BAD: [retrieve("plan creation"), search(query=productivity)]
        BAD: ["How to", "Missing complete questions"]
        BAD: Recommendations: ["..."] (extra text outside array)
        
        ### Generate {self.n_prompt_recommendations} recommendations as a JSON array:
        '''

        raw_recommendations = (self.workflow.invoke({"messages": [{"role": "user", "content": input_message}]},
                                                    stream_mode='values',
                                                    config=self.chat_id
                                                    )['messages'][-1].content
                               )
        sanitized = re.sub(r'^[^\[]*', '', raw_recommendations)
        sanitized = re.sub(r'[^\]]*$', '', sanitized)
        sanitized = sanitized.replace('\n', ' ').replace("'", '"')

        recommended_prompts = ['']
        try:
            sanitized = re.sub(r',\s*]', ']', sanitized)
            if isinstance(json.loads(sanitized), list):
                recommended_prompts = json.loads(sanitized)
        except Exception as e:
            pass

        return langchain_pb2.RecommendedPrompts(**{'recommended_prompts': recommended_prompts})


class TagRecommender(tag_recommender_pb2_grpc.TagRecommenderServicer):

    def __init__(self, config):
        super().__init__()
        self.logger = logging.getLogger(self.__class__.__name__)
        self.model_path = 'tag_recommender'
        self.tag_recommender = None
        self.embeddings_model = init_embeddings(config['embeddings_model'])
        self.vector_store = Chroma(
            collection_name='note_embeddings',
            embedding_function=self.embeddings_model,
            persist_directory="./note_embeddings",
        )

        if os.path.isfile(self.model_path):
            self.tag_recommender = Top2Vec.load('tag_recommender')
        else:
            try:
                notes = self.vector_store.get()['documents']
                self.tag_recommender = Top2Vec(documents=notes, min_count=1, embedding_model='doc2vec', keep_documents=False)
                self.tag_recommender.save('tag_recommender')
            except Exception as tag_recommender_learning_exception:
                pass

    def RecommendTags(self, request, context):
        existing_tags = list(request.existing_tags)
        if not existing_tags:
            return tag_recommender_pb2.RecommendedTags(**{'recommended_tags': ['']})

        self.logger.info(f"Existing tags: {request.existing_tags}")
        self.logger.info(f'Request note id: {request.note_id}')
        self.logger.info(f"Existing notes\' ids: {self.vector_store.get()['ids']}")
        query = self.vector_store.get(str(request.note_id))['documents'][0]
        query_embedding = self.embeddings_model.embed_query(query)
        existing_topics_embeddings = self.embeddings_model.embed_documents(existing_tags)
        existing_tags_relevance_scores = cosine_similarity(existing_topics_embeddings, np.array(query_embedding).reshape(1, -1))
        existing_relevance_scores_and_tags = list(zip(existing_tags_relevance_scores, existing_tags))
        all_relevance_scores_and_tags = existing_relevance_scores_and_tags
        self.logger.info('Existing tags proceeded succesfully')

        if os.path.isfile(self.model_path):
            top2vec_num_topics = self.tag_recommender.get_num_topics()
            top2vec_recommendations = self.tag_recommender.query_topics(query, reduced=False, num_topics=top2vec_num_topics)
            top2vec_topics = []
            top2vec_topics_scores = []
            self.logger.info('Processing Top2Vec topics')
            for i in range(top2vec_num_topics):
                top2vec_topics += list(top2vec_recommendations[0][i][:5])
            self.logger.info('Processing Top2Vec scores')
            for i in range(top2vec_num_topics):
                top2vec_topics_scores += list(top2vec_recommendations[1][i][:5])
            top2vec_relevance_scores_and_tags = list(zip(top2vec_topics, top2vec_topics_scores))
            all_relevance_scores_and_tags += top2vec_relevance_scores_and_tags

        all_relevance_scores_and_tags.sort()

        best_tags = []
        self.logger.info('Processing recommendations')
        for recommendation in all_relevance_scores_and_tags[-1:-6:-1]:
            best_tags.append(recommendation[1])

        return tag_recommender_pb2.RecommendedTags(**{'recommended_tags': best_tags})



def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    with open("config.json", "r", encoding="utf-8") as f:
        config = json.load(f)
    langchain_pb2_grpc.add_LangChainServicer_to_server(LangChainService(config), server)
    sync_pb2_grpc.add_SyncServiceServicer_to_server(SyncService(config), server)
    tag_recommender_pb2_grpc.add_TagRecommenderServicer_to_server(TagRecommender(config), server)
    server.add_insecure_port("0.0.0.0:8090")
    server.start()
    server.wait_for_termination()

if __name__ == "__main__":
    serve()
