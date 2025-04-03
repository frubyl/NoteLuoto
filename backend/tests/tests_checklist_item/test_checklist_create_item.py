import pytest
from testsuite.databases import pgsql

async def test_create_item_in_nonexistent_checklist(service_client, auth_header):
    """Тест проверяет создание элемента в несуществующем чеклисте"""
    response = await service_client.post(
        "/checklist/1/item",
        json={"text": "text"},
        headers=auth_header
    )
    
    assert response.status == 404

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_create_item_in_checklist_success(service_client, auth_header):
    """Тест проверяет успешное создание элемента в существующем чеклисте"""
    response = await service_client.post(
        "/checklist/1/item",
        json={"text": "text"},
        headers=auth_header
    )
    
    assert response.status == 201
    assert response.json()["item_id"] == 3