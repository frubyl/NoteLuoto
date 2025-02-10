import pytest
from testsuite.databases import pgsql

# Нет заметки
async def test_patch_checklist_1(service_client, auth_header):
    response = await service_client.patch("/checklist/99",
                                        json = {"title": "title"},
                                         headers = auth_header)
    assert response.status == 404
# Все ок
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_patch_checklist_2(service_client, auth_header):
    response = await service_client.patch("/checklist/1",
                                        json = {"title": "new"},
                                         headers = auth_header)
    assert response.status == 200

    
