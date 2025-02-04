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
async def test_note_delete_tag_1(service_client, auth_header):
    response = await service_client.delete("/tags/99/99",
                                         headers = auth_header)
    assert response.status == 404

# Нет тега
async def test_note_delete_tag_2(service_client, auth_header):
    response = await service_client.delete("/tags/99/99",
                                         headers = auth_header)
    assert response.status == 404

# Тег не добавлен к заметке
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_note_delete_tag_3(service_client, auth_header):
    response = await service_client.delete("/tags/1/1",
                                         headers = auth_header)
    assert response.status == 404

# Все ок
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_note_delete_tag_4(service_client, auth_header):
    response = await service_client.delete("/tags/1/2",
                                         headers = auth_header)
    assert response.status == 200
