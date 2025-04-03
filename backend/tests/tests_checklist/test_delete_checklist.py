import pytest
from testsuite.databases import pgsql

async def test_delete_nonexistent_checklist(service_client, auth_header):
    """Тест проверяет удаление несуществующего чеклиста"""
    response = await service_client.delete(
        "/checklist/99",
        headers=auth_header
    )
    assert response.status == 404

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_delete_checklist_success(service_client, auth_header):
    """Тест проверяет успешное удаление существующего чеклиста"""
    response = await service_client.delete(
        "/checklist/1",
        headers=auth_header
    )
    assert response.status == 200
