import pathlib
import sys

import pytest
import grpc

import samples.Langchain_pb2 as langchain_protos

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_ai_answer_success(service_client, mock_grpc_langchain, auth_header):
    """Тест успешного получения ответа бота"""
    @mock_grpc_langchain('GenerateAnswer')
    async def mock_generate_answer(request, context):
        return langchain_protos.ChatbotAnswer(
            answer_text="Answer"
        )

    response = await service_client.post(
        '/ai/answer',
        json= {"question": "question"},
        headers=auth_header
    )
    
    assert response.status == 200
    assert response.json() ==  {"answer" : "Answer"}
    assert mock_generate_answer.times_called == 1

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_ai_answer_empty(service_client, mock_grpc_langchain, auth_header):
    """Тест пустого ответа"""
    @mock_grpc_langchain('GenerateAnswer')
    async def mock_generate_answer(request, context):
        return langchain_protos.ChatbotAnswer(
            answer_text=""
        )

    response = await service_client.post(
        '/ai/answer',
        json= {"question": "question"},
        headers=auth_header
    )
    
    assert response.status == 200
    assert response.json() ==  {"answer" : ""}
    assert mock_generate_answer.times_called == 1

async def test_ai_answer_grpc_error(service_client, mock_grpc_langchain, auth_header):
    """Тест ошибки gRPC сервиса"""
    @mock_grpc_langchain('GenerateAnswer')
    async def mock_generate_answer(request, context):
        raise _AbortionError(StatusCode.UNAVAILABLE, "Service unavailable")

    response = await service_client.post(
        '/ai/answer',
        json= {"question": "question"},
        headers=auth_header
    )
    
    assert response.status == 500
    assert 'Internal Server Error' in response.json()['message']
    assert mock_generate_answer.times_called == 1

async def test_ai_answer_empty_question(service_client, mock_grpc_langchain, auth_header):
    """Тест пустого вопроса"""
    @mock_grpc_langchain('GenerateAnswer')
    async def mock_generate_answer(request, context):
        return langchain_protos.ChatbotAnswer(
            answer_text=""
        )

    response = await service_client.post(
        '/ai/answer',
        json= {"question": ""},
        headers=auth_header
    )
    
    assert response.status == 400

async def test_ai_answer_question_max_lenght(service_client, mock_grpc_langchain, auth_header):
    """Тест большого вопроса"""
    @mock_grpc_langchain('GenerateAnswer')
    async def mock_generate_answer(request, context):
        return langchain_protos.ChatbotAnswer(
            answer_text=""
        )

    response = await service_client.post(
        '/ai/answer',
        json= {"question": "dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl'dfgdfdfgdfgdfgdfgdfgdfgdfgjkjklkljkljkljkljkljkljkldfgdfgdfgdfgdfgdffgdfgdfgdfgdfgdfgdfgdfgdfgjkljkl"},
        headers=auth_header
    )
    
    assert response.status == 400

async def test_ai_answer_invalid_token(service_client, mock_grpc_langchain):
    """Тест невалидного токена"""
    @mock_grpc_langchain('GenerateAnswer')
    async def mock_generate_answer(request, context):
        return langchain_protos.ChatbotAnswer(
            answer_text=""
        )

    response = await service_client.post(
        '/ai/answer',
        json= {"question": "t"}    )
    
    assert response.status == 401