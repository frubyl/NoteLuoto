import pytest
from testsuite.databases import pgsql

async def test_update_nonexistent_item(service_client, auth_header):
    """Тест проверяет обновление несуществующего элемента чеклиста"""
    response = await service_client.patch(
        "/checklist/item/99",
        json={"text": "text", "status": True},
        headers=auth_header
    )
    assert response.status == 404

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_update_item_text_only(service_client, auth_header):
    """Тест проверяет обновление только текста элемента"""
    response = await service_client.patch(
        "/checklist/item/1",
        json={"text": "text"},
        headers=auth_header
    )
    assert response.status == 200

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_update_item_status_only(service_client, auth_header):
    """Тест проверяет обновление только статуса элемента"""
    response = await service_client.patch(
        "/checklist/item/1",
        json={"status": True},
        headers=auth_header
    )
    assert response.status == 200

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_update_item_text_and_status(service_client, auth_header):
    """Тест проверяет обновление и текста, и статуса элемента"""
    response = await service_client.patch(
        "/checklist/item/1",
        json={"text": "text", "status": True},
        headers=auth_header
    )
    assert response.status == 200