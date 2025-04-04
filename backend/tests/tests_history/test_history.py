import pytest
from testsuite.databases import pgsql

async def test_empty_history(service_client, auth_header):
    """Тест проверяет получение пустой истории запросов"""
    response = await service_client.get(
        "/history",
        headers=auth_header
    )
    assert response.status == 200
    assert response.json() == {"history": []}

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_full_history_single_page(service_client, auth_header):
    """Тест проверяет получение полной истории на одной странице"""
    response = await service_client.get(
        "/history",
        headers=auth_header
    )
    expected = {
        "history": [
             {
                "query_id": 6,
                "query": "Sixth query",
                "response": "Sixth response",
                "created_at": "2025-03-10T12:30:00+00:00",
            },
             {
                "query_id": 5,
                "query": "Fifth query",
                "response": "Fifth response",
                "created_at": "2025-03-10T11:32:00+00:00",
            },
             {
                "query_id": 4,
                "query": "Fourth query",
                "response": "Fourth response",
                "created_at": "2025-03-10T11:31:00+00:00",
            },
            {
                "query_id": 3,
                "query": "Third query",
                "response": "Third response",
                "created_at": "2025-03-10T11:30:00+00:00",
            },
            {
                "query_id": 2,
                "query": "Second query",
                "response": "Second response",
                "created_at": "2025-03-10T10:30:00+00:00",
            },
            {
                "query_id": 1,
                "query": "First query",
                "response": "First response",
                "created_at": "2025-03-10T09:30:00+00:00",
            }
        ]
    }
    assert response.status == 200
    assert response.json() == expected




@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_paginated_history(service_client, auth_header):
    """Тест проверяет постраничное получение истории запросов"""
    response = await service_client.get(
        "/history",
        params={"page": "2", "limit": "5"},
        headers=auth_header
    )
    expected = {
        "history": [
            {
                "query_id": 1,
                "query": "First query",
                "response": "First response",
                "created_at": "2025-03-10T09:30:00+00:00",
            }
        ]
    }
    assert response.status == 200
    assert response.json() == expected


async def test_history_invalid_page(service_client, auth_header):
    """Тест проверяет невалидную страницу"""
    response = await service_client.get(
        "/history",
        params={"page": "-1", "limit": "1"},
        headers=auth_header
    )
    assert response.status == 400

async def test_history_invalid_limit(service_client, auth_header):
    """Тест проверяет невалидный лимит"""
    response = await service_client.get(
        "/history",
        params={"page": "1", "limit": "-1"},
        headers=auth_header
    )
    assert response.status == 400

async def test_history_invalid_token(service_client):
    """Тест проверяет невалидный токен"""
    response = await service_client.get(
        "/history",
        params={"page": "1", "limit": "1"},
    )
    assert response.status == 401
