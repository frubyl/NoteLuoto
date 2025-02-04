import pytest
from testsuite.databases import pgsql

@pytest.fixture
def auth_header():
    token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ0b2tlbl9leHBfdGltZSI6MTczODgzNzEwMiwidXNlcl9pZCI6MX0.SdRRnUsbmQeALGIejJI8Whf0KAqzBHLzTNcNw2LH9z0"
    headers = {
        "Authorization": f"Token {token}"
    }
    return headers


# Тестирование создания заметки
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_create_note(service_client, auth_header):
    response = await service_client.post("/notes",
                                         json={"title": "frubyl",
                                                 "body": "frubasik"}, 
                                        headers = auth_header)
    assert response.status == 201
    assert response.json()["note_id"] == 2

