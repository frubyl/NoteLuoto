import grpc
import sync_pb2
import sync_pb2_grpc
import langchain_pb2
import langchain_pb2_grpc

from langchain_chroma import Chroma
from langchain_ollama import ChatOllama, OllamaEmbeddings
from langchain_core.documents import Document
from langchain_text_splitters import RecursiveCharacterTextSplitter
from langchain_core.tools import tool
from langchain_core.messages import SystemMessage
from langgraph.graph import MessagesState, StateGraph, END
from langgraph.prebuilt import ToolNode, tools_condition


class SyncService(sync_pb2_grpc.SyncServiceServicer):

    def __init__(self):
        super().__init__()
        self.embeddings_model = OllamaEmbeddings(model="llama3.2:3b") # change to model from config
        self.vector_store = Chroma(
            collection_name='note_embeddings',
            embedding_function=self.embeddings_model,
            persist_directory="./note_embeddings",
        )
        self.text_splitter = RecursiveCharacterTextSplitter(chunk_size=1000, chunk_overlap=200)

    def CreateNote(self, request, context):
        note_id = request.note_id
        note_text = request.full_text
        note = Document(page_content=note_text)
        note_split = self.text_splitter.split_documents([note])
        self.vector_store.add_documents(documents=note_split, ids=str(note_id)) # add try except

        return sync_pb2.EmptyResponse()

    def UpdateNote(self, request, context):
        note_id = request.note_id
        note_text = request.full_text
        note = Document(page_content=note_text)
        self.vector_store.update_document(str(note_id), note) #add try except

        return sync_pb2.EmptyResponse()

    def DeleteNote(self, request, context):
        note_id = request.note_id
        self.vector_store.delete([str(note_id)]) #add try except

        return sync_pb2.EmptyResponse()

class LangChainService(langchain_pb2_grpc.LangChainServicer):

    def __init__(self):
        super().__init__()
        self.graph_builder = StateGraph(MessagesState)
        self.llm = ChatOllama(model="llama3.2:3b")
        self.vector_store = Chroma(
            collection_name='note_embeddings',
            embedding_function=self.embeddings_model,
            persist_directory="./note_embeddings",
        )

    @tool(response_format="content_and_artifact")
    def retrieve(self, query: str):
        retrieved_docs = self.vector_store.similarity_search(query, k=2)
        serialized = "\n".join((f"Content: {doc.page_content}") for doc in retrieved_docs)
        return serialized, retrieved_docs

    def query_or_respond(self, state: MessagesState):
        llm_with_tools = self.llm.bind_tools([self.retrieve])
        response = llm_with_tools.invoke(state["messages"])
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
            "You are an assistant for question-answering tasks. "
            "Use the following pieces of retrieved context to answer "
            "the question. If you don't know the answer, say that you "
            "don't know. Use three sentences maximum and keep the "
            "answer concise."
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

        response = self.llm.invoke(prompt)
        return {"messages": [response]}

    def initialize_generation_graph(self):
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

        self.generation_graph = self.graph_builder.compile()

    def AnswerQuery(self, request, context):
        query = request.query_text
        query_answer = (self.generation_graph.invoke({"messages": [{"role": "user", "content": query}]}, stream_mode='values')
                        ['messages'][-1].content)
        return langchain_pb2.QueryAnswer(**{'answer_text': query_answer})

def serve():
    server = grpc.server()
    sync_pb2_grpc.add_SyncServiceServicer_to_server(SyncService(), server)
    langchain_pb2_grpc.add_LangChainServicer_to_server(LangChainService(), server)
    server.add_insecure_port("localhost:8090")
    server.start()
    server.wait_for_termination()

if __name__ == "__main__":
    serve()
