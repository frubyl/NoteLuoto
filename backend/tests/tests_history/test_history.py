import pytest
from testsuite.databases import pgsql

async def test_empty_history(service_client, auth_header):
    """Тест проверяет получение пустой истории запросов"""
    response = await service_client.get(
        "/history",
        params={"page": "1", "limit": "1"},
        headers=auth_header
    )
    assert response.status == 200
    assert response.json() == {"history": []}

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_full_history_single_page(service_client, auth_header):
    """Тест проверяет получение полной истории на одной странице"""
    response = await service_client.get(
        "/history",
        params={"page": "1", "limit": "3"},
        headers=auth_header
    )
    expected = {
        "history": [
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
        params={"page": "2", "limit": "1"},
        headers=auth_header
    )
    expected = {
        "history": [
            {
                "query_id": 2,
                "query": "Second query",
                "response": "Second response",
                "created_at": "2025-03-10T10:30:00+00:00",
            }
        ]
    }
    assert response.status == 200
    assert response.json() == expected