import pytest
from testsuite.databases import pgsql

async def test_delete_nonexistent_item(service_client, auth_header):
    """Тест проверяет удаление несуществующего элемента чеклиста"""
    response = await service_client.delete("/checklist/item/1",
                                         headers=auth_header)
    assert response.status == 404

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_delete_item_success(service_client, auth_header):
    """Тест проверяет успешное удаление элемента чеклиста"""
    response = await service_client.delete("/checklist/item/1",
                                         headers=auth_header)
    assert response.status == 200

async def test_delete_item_invalid_checklist_id(service_client, auth_header):
    """Тест проверяет невалидный id"""
    response = await service_client.delete("/checklist/item/-1",
                                         headers=auth_header)
    assert response.status == 400

async def test_delete_item_invalid_token(service_client):
    """Тест проверяет невалидный токен"""
    response = await service_client.delete("/checklist/item/1")
    assert response.status == 401