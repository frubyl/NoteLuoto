import pytest
from testsuite.databases import pgsql

@pytest.fixture
def auth_header():
    token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ0b2tlbl9leHBfdGltZSI6MTczODgzNzEwMiwidXNlcl9pZCI6MX0.SdRRnUsbmQeALGIejJI8Whf0KAqzBHLzTNcNw2LH9z0"
    headers = {
        "Authorization": f"Token {token}"
    }
    return headers


# Нет заметки
async def test_note_tags_1(service_client, auth_header):
    response = await service_client.get("/tags/1",
                                         headers = auth_header)
    assert response.status == 404


# Получение всех тегов
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_note_tags_2(service_client, auth_header):
    response = await service_client.get("/tags/1",
                                         headers = auth_header)
    expected = [
        {
            "tag_id": 2,
            "name": "tag2"
        }
    ]
    assert response.status == 200
    assert response.json() == expected
