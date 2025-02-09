import pytest
from testsuite.databases import pgsql

@pytest.fixture
def auth_header():
    token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ0b2tlbl9leHBfdGltZSI6MTczODgzNzEwMiwidXNlcl9pZCI6MX0.SdRRnUsbmQeALGIejJI8Whf0KAqzBHLzTNcNw2LH9z0"
    headers = {
        "Authorization": f"Token {token}"
    }
    return headers


# Нет истории
async def test_history_1(service_client, auth_header):
    response = await service_client.get("/history",
                                        params={"page": "1",
                                                 "limit": "1"},
                                       headers=auth_header)
    expected = {"history":[]}

    assert response.status == 200
    assert response.json() == expected


# Вся история на одной странице
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_history_3(service_client, auth_header):
    response = await service_client.get("/history",
                                        params={"page": "1",
                                                 "limit": "3"},
                                        headers=auth_header)
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



# Вся история на нескольких страницах
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_history_4(service_client, auth_header):
    response = await service_client.get("/history",
                                        params={"page": "2",
                                                 "limit": "1"},
                                       headers=auth_header)
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
