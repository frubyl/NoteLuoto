import pytest
from testsuite.databases import pgsql

async def test_update_nonexistent_checklist(service_client, auth_header):
    """Тест проверяет обновление несуществующего чеклиста"""
    response = await service_client.patch("/checklist/99",
                                        json = {"title": "title"},
                                        headers = auth_header)

    assert response.status == 404

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_update_checklist_success(service_client, auth_header):
    """Тест проверяет успешное обновление существующего чеклиста"""
    response = await service_client.patch("/checklist/1",
                                        json = {"title": "new"},
                                        headers = auth_header)
    assert response.status == 200
