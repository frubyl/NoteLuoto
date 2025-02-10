import pytest
from testsuite.databases import pgsql

# Нет заметки
async def test_delete_checklist_1(service_client, auth_header):
    response = await service_client.delete("/checklist/99",
                                         headers = auth_header)
    assert response.status == 404
# Все ок
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_delete_checklist_2(service_client, auth_header):
    response = await service_client.delete("/checklist/1",
                                         headers = auth_header)
    assert response.status == 200

    
