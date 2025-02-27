import pytest
from testsuite.databases import pgsql


# Тестирование создания заметки
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_create_note(service_client, auth_header):
    response = await service_client.post("/note",
                                         json={"title": "frubyl",
                                                 "body": "frubasik"}, 
                                        headers = auth_header)
    assert response.status == 201
    assert response.json()["note_id"] == 2

