import pytest
from unittest.mock import AsyncMock, MagicMock
import grpc
from samples.Langchain_pb2 import (
    UserQuery,
    ChatbotAnswer,
    SemanticSearchResult,
    EmptyRequest,
    RecommendedPrompts
)
from samples.Langchain_pb2_grpc import LangChainStub

@pytest.fixture
def mock_grpc_client():
    """Фикстура для мока gRPC клиента LangChain"""
    client = MagicMock(spec=LangChainStub)
    
    # Настраиваем моки для всех методов
    client.GenerateAnswer = AsyncMock(
        return_value=ChatbotAnswer(answer_text="Test answer")
    )
    client.SemanticSearch = AsyncMock(
        return_value=SemanticSearchResult(notes_id=[1, 2, 3])
    )
    client.RecommendPrompts = AsyncMock(
        return_value=RecommendedPrompts(recommended_prompts=["Prompt 1", "Prompt 2"])
    )
    
    return client

async def test_grpc_langchain_client_generate_answer_success(mock_grpc_client):
    """Тестирование успешного ответа GenerateAnswer gRPC клиента LangChain"""
    request = UserQuery(query_text="Test question")
    
    response = await mock_grpc_client.GenerateAnswer(request)
    
    assert isinstance(response, ChatbotAnswer)
    assert response.answer_text == "Test answer"
    mock_grpc_client.GenerateAnswer.assert_called_once_with(request)


async def test_grpc_langchain_client_semantic_search_success(mock_grpc_client):
    """Тестирование успешного ответа SemanticSearch gRPC клиента LangChain"""
    request = UserQuery(query_text="Search query")
    
    response = await mock_grpc_client.SemanticSearch(request)
    
    assert isinstance(response, SemanticSearchResult)
    assert response.notes_id == [1, 2, 3]
    mock_grpc_client.SemanticSearch.assert_called_once_with(request)

async def test_grpc_langchain_client_semantic_search_empty_result(mock_grpc_client):
    """Тестирование пустого ответа SemanticSearch gRPC клиента LangChain"""
    mock_grpc_client.SemanticSearch.return_value = SemanticSearchResult(notes_id=[])
    
    response = await mock_grpc_client.SemanticSearch(UserQuery(query_text="Empty"))
    
    assert len(response.notes_id) == 0
    mock_grpc_client.SemanticSearch.assert_called_once()

async def test_grpc_langchain_client_recommend_prompts_success(mock_grpc_client):
    """Тестирование успешного ответа RecommendPrompts gRPC клиента LangChain"""
    response = await mock_grpc_client.RecommendPrompts(EmptyRequest())
    
    assert isinstance(response, RecommendedPrompts)
    assert response.recommended_prompts == ["Prompt 1", "Prompt 2"]
    mock_grpc_client.RecommendPrompts.assert_called_once_with(EmptyRequest())

async def test_grpc_langchain_client_recommend_prompts_custom_response(mock_grpc_client):
    """Тестирование кастомного ответа RecommendPrompts gRPC клиента LangChain"""
    custom_prompts = ["Custom 1", "Custom 2", "Custom 3"]
    mock_grpc_client.RecommendPrompts.return_value = RecommendedPrompts(
        recommended_prompts=custom_prompts
    )
    
    response = await mock_grpc_client.RecommendPrompts(EmptyRequest())
    
    assert len(response.recommended_prompts) == 3
    assert "Custom 2" in response.recommended_prompts
    mock_grpc_client.RecommendPrompts.assert_called_once()