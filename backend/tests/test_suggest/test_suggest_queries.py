import pathlib
import sys

import pytest
import grpc

import samples.Langchain_pb2 as langchain_protos

async def test_suggest_queries_success(service_client, mock_grpc_langchain, auth_header):
    """Тест успешного получения промптов"""
    @mock_grpc_langchain('RecommendPrompts')
    async def mock_recommend_prompts(request, context):
        return langchain_protos.RecommendedPrompts(
            recommended_prompts=['Prompt 1', 'Prompt 2']
        )

    response = await service_client.get(
        '/suggest/queries',
        headers=auth_header
    )
    
    assert response.status == 200
    assert response.json() ==  ['Prompt 1', 'Prompt 2']
    assert mock_recommend_prompts.times_called == 1

async def test_suggest_queries_empty(service_client, mock_grpc_langchain, auth_header):
    """Тест пустого ответа"""
    @mock_grpc_langchain('RecommendPrompts')
    async def mock_recommend_prompts(request, context):
        return langchain_protos.RecommendedPrompts(
            recommended_prompts=[]
        )

    response = await service_client.get(
        '/suggest/queries',
        headers=auth_header
    )
    
    assert response.status == 200
    assert response.json() == []
    assert mock_recommend_prompts.times_called == 1

async def test_suggest_queries_grpc_error(service_client, mock_grpc_langchain, auth_header):
    """Тест ошибки gRPC сервиса"""
    @mock_grpc_langchain('RecommendPrompts')
    async def mock_recommend_prompts(request, context):
        raise _AbortionError(StatusCode.UNAVAILABLE, "Service unavailable")

    response = await service_client.get(
        '/suggest/queries',
        headers=auth_header
    )
    
    assert response.status == 500
    assert 'Internal Server Error' in response.json()['message']
    assert mock_recommend_prompts.times_called == 1


async def test_suggest_queries_invalid_token(service_client, mock_grpc_langchain):
    """Тест неверного токена"""
    @mock_grpc_langchain('RecommendPrompts')
    async def mock_recommend_prompts(request, context):
        return langchain_protos.RecommendedPrompts(
            recommended_prompts=[]
        )

    response = await service_client.get(
        '/suggest/queries'    )
    
    assert response.status == 401