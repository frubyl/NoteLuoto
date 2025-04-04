import pathlib
import sys

import pytest
import grpc

import samples.TagRecommender_pb2 as tag_recommender_protos

async def test_suggest_tags_success(service_client, mock_grpc_tag_recommender, auth_header):
    """Тест успешного получения тегов"""
    @mock_grpc_tag_recommender('RecommendTags')
    async def mock_recommend_tags(request, context):
        return tag_recommender_protos.RecommendedTags(
            recommended_tags=['Tag 1', 'Tag 2']
        )

    response = await service_client.post(
        '/suggest/tags/1',
        headers=auth_header
    )
    
    assert response.status == 200
    assert response.json() == ['Tag 1', 'Tag 2']
    assert mock_recommend_tags.times_called == 1

async def test_suggest_tags_empty(service_client, mock_grpc_tag_recommender, auth_header):
    """Тест пустого ответа"""
    @mock_grpc_tag_recommender('RecommendTags')
    async def mock_recommend_tags(request, context):
        return tag_recommender_protos.RecommendedTags(
            recommended_tags=[]
        )

    response = await service_client.post(
        '/suggest/tags/1',
        headers=auth_header
    )
    
    assert response.status == 200
    assert response.json() == []
    assert mock_recommend_tags.times_called == 1

async def test_suggest_tags_grpc_error(service_client, mock_grpc_tag_recommender, auth_header):
    """Тест ошибки gRPC сервиса"""
    @mock_grpc_tag_recommender('RecommendTags')
    async def mock_recommend_tags(request, context):
        raise _AbortionError(StatusCode.UNAVAILABLE, "Service unavailable")

    response = await service_client.post(
        '/suggest/tags/1',
        headers=auth_header
    )
    
    assert response.status == 500
    assert 'Internal Server Error' in response.json()['message']
    assert mock_recommend_tags.times_called == 1

async def test_suggest_tags_invalid_token(service_client, mock_grpc_tag_recommender):
    """Тест неверный токен"""
    @mock_grpc_tag_recommender('RecommendTags')
    async def mock_recommend_tags(request, context):
        return tag_recommender_protos.RecommendedTags(
            recommended_tags=[]
        )
    response = await service_client.post(
        '/suggest/tags/1',
    )
    
    assert response.status == 401

async def test_suggest_tags_invalid_note_id(service_client, mock_grpc_tag_recommender, auth_header):
    """Тест неверный токен"""
    @mock_grpc_tag_recommender('RecommendTags')
    async def mock_recommend_tags(request, context):
        return tag_recommender_protos.RecommendedTags(
            recommended_tags=[]
        )
    response = await service_client.post(
        '/suggest/tags/-1',
        headers=auth_header
    )
    
    assert response.status == 400
