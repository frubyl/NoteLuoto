import pytest
from testsuite.databases import pgsql


# Нет заметки
async def test_checklist_create_item_1(service_client, auth_header):
    response = await service_client.post("/checklist/1/item",
                                        json = {"text": "text"},
                                         headers = auth_header)
    assert response.status == 404

# Все ок
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_checklist_create_item_2(service_client, auth_header):
    response = await service_client.post("/checklist/1/item",
                                        json = {"text": "text"},
                                         headers = auth_header)
    assert response.status == 201
    assert response.json()["item_id"]== 3

    
