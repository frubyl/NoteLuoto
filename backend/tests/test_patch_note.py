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

async def test_patch_note_1(service_client, auth_header):
    response = await service_client.patch("/notes/999",
                                            json = {"title": "new"},
                                         headers = auth_header)
    assert response.status == 404

# Обновляется только title
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_patch_note_2(service_client, auth_header):
    response = await service_client.patch("/notes/1",
                                         json={"title": "new"},
                                          headers = auth_header)
    assert response.status == 200


# Обновляется только body
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_patch_note_3(service_client, auth_header):
    response = await service_client.patch("/notes/1",
                                         json={"body": "new"},
                                          headers = auth_header)
    assert response.status == 200


# Обновляется body и title
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_patch_note_4(service_client, auth_header):
    response = await service_client.patch("/notes/1",
                                         json={"body": "new",
                                         "title": "new"},
                                          headers = auth_header)
    assert response.status == 200

