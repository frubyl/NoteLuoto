import pytest
from unittest.mock import AsyncMock, MagicMock
import grpc
from samples.TagRecommender_pb2 import (
    TagRecommendationRequest,
    RecommendedTags
)
from samples.TagRecommender_pb2_grpc import TagRecommenderStub

@pytest.fixture
def mock_tag_recommender_client():
    """Фикстура для мока gRPC клиента TagRecommender"""
    client = MagicMock(spec=TagRecommenderStub)
    
    # Настраиваем мок для метода RecommendTags
    client.RecommendTags = AsyncMock(
        return_value=RecommendedTags(recommended_tags=["tag1", "tag2", "tag3"])
    )
    
    return client

async def test_grpc_tag_recommender_recommend_tags_success(mock_tag_recommender_client):
    """Тестирование успешного получения рекомендаций тегов"""
    request = TagRecommendationRequest(
        note_id=123,
        existing_tags=["existing1", "existing2"]
    )
    
    response = await mock_tag_recommender_client.RecommendTags(request)
    
    assert isinstance(response, RecommendedTags)
    assert len(response.recommended_tags) == 3
    assert "tag1" in response.recommended_tags
    mock_tag_recommender_client.RecommendTags.assert_called_once_with(request)

async def test_grpc_tag_recommender_recommend_tags_empty_response(mock_tag_recommender_client):
    """Тестирование пустого ответа от сервиса рекомендаций"""
    mock_tag_recommender_client.RecommendTags.return_value = RecommendedTags(recommended_tags=[])
    
    request = TagRecommendationRequest(note_id=456, existing_tags=[])
    response = await mock_tag_recommender_client.RecommendTags(request)
    
    assert len(response.recommended_tags) == 0
    mock_tag_recommender_client.RecommendTags.assert_called_once_with(request)


async def test_grpc_tag_recommender_recommend_tags_without_existing(mock_tag_recommender_client):
    """Тестирование запроса без существующих тегов"""
    request = TagRecommendationRequest(note_id=321, existing_tags=[])
    
    response = await mock_tag_recommender_client.RecommendTags(request)
    
    assert len(response.recommended_tags) == 3
    mock_tag_recommender_client.RecommendTags.assert_called_once_with(request)

async def test_grpc_tag_recommender_recommend_tags_custom_response(mock_tag_recommender_client):
    """Тестирование с кастомным набором тегов в ответе"""
    custom_tags = ["custom1", "custom2"]
    mock_tag_recommender_client.RecommendTags.return_value = RecommendedTags(
        recommended_tags=custom_tags
    )
    
    request = TagRecommendationRequest(note_id=555, existing_tags=["old"])
    response = await mock_tag_recommender_client.RecommendTags(request)
    
    assert len(response.recommended_tags) == 2
    assert "custom1" in response.recommended_tags
    mock_tag_recommender_client.RecommendTags.assert_called_once_with(request)