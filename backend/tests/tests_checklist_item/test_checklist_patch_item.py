import pytest
from testsuite.databases import pgsql

# Нет айтема
async def test_checklist_patch_item_1(service_client, auth_header):
    response = await service_client.patch("/checklist/item/99",
                                        json = {"text": "text",
                                                "status": True},
                                         headers = auth_header)
    assert response.status == 404

# Обновляется только тело
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_checklist_patch_item_2(service_client, auth_header):
    response = await service_client.patch("/checklist/item/1",
                                        json = {"text": "text"},
                                         headers = auth_header)
    assert response.status == 200

    
# Обновляется только статус
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_checklist_patch_item_3(service_client, auth_header):
    response = await service_client.patch("/checklist/item/1",
                                        json = {"status": True},
                                         headers = auth_header)
    assert response.status == 200

    
# Обновляется только тело и статус
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_checklist_patch_item_4(service_client, auth_header):
    response = await service_client.patch("/checklist/item/1",
                                        json = {"text": "text",
                                                "status": True},
                                         headers = auth_header)
    assert response.status == 200

    
