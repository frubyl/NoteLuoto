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


async def test_update_checklist_invalid_checklist_id(service_client, auth_header):
    """Тест проверяет невалидный айди чеклиста"""
    response = await service_client.patch("/checklist/-1",
                                        json = {"title": "title"},
                                        headers = auth_header)

    assert response.status == 400

    
async def test_update_checklist_invalid_title(service_client, auth_header):
    """Тест проверяет невалидный заголовок"""
    response = await service_client.patch("/checklist/99",
                                        json = {"title": ""},
                                        headers = auth_header)

    assert response.status == 400

async def test_update_checklist_invalid_token(service_client):
    """Тест проверяет невалидный токен"""
    response = await service_client.patch("/checklist/99",
                                        json = {"title": "title"})

    assert response.status == 401