import pytest
from testsuite.databases import pgsql

# Нет заметки с таким айди
async def test_get_one_note_1(service_client, auth_header):
    response = await service_client.get("/notes/999",
                                         headers = auth_header)
    assert response.status == 404

# Есть заметка с таким айди
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_get_one_note_2(service_client, auth_header):
    response = await service_client.get("/notes/1",
                                         headers = auth_header)
    expected = {"title": "title",
                "body": "body",
                "created_at": "2025-03-10T09:30:00+00:00", 
                "updated_at": "2025-03-10T09:30:00+00:00"
    }

    assert response.status == 200
    assert response.json() == expected

