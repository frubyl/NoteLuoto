import pytest
from testsuite.databases import pgsql

# Нет заметки
async def test_note_add_checklist_1(service_client, auth_header):
    response = await service_client.post("/checklist/99",
                                        json = {"title": "title"},
                                         headers = auth_header)
    assert response.status == 404
# Все ок
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_note_add_checklist_2(service_client, auth_header):
    response = await service_client.post("/checklist/1",
                                        json = {"title": "title"},
                                         headers = auth_header)
    assert response.status == 201
    assert response.json()["checklist_id"]== 2

    
