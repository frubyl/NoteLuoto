import pytest
from testsuite.databases import pgsql

async def test_note_add_tag_note_doesnt_exist(service_client, auth_header):
    """Тест проверяет добавление тега к несуществующей заметке"""
    response = await service_client.post("/tags/99/99",
                                         headers = auth_header)
    assert response.status == 404

async def test_note_add_tag_doesnt_exist(service_client, auth_header):
    """Тест проверяет добавление несуществующего тега"""
    response = await service_client.post("/tags/99/99",
                                         headers = auth_header)
    assert response.status == 404

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_note_add_tag_already_add(service_client, auth_header):
    """Тест проверяет добавление тега который уже добавлен к заметке"""
    response = await service_client.post("/tags/1/2",
                                         headers = auth_header)
    assert response.status == 409

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_note_add_tag_success(service_client, auth_header):
    """Тест проверяет успешное добавление тега"""
    response = await service_client.post("/tags/1/1",
                                         headers = auth_header)
    assert response.status == 200

async def test_note_add_tag_invalid_note_id(service_client, auth_header):
    """Тест проверяет неверный id заметки"""
    response = await service_client.post("/tags/-1/1",
                                         headers = auth_header)
    assert response.status == 400

async def test_note_add_tag_invalid_tag_id(service_client, auth_header):
    """Тест проверяет неверный id тега"""
    response = await service_client.post("/tags/1/-1",
                                         headers = auth_header)
    assert response.status == 400

async def test_note_add_tag_invalid_token(service_client):
    """Тест проверяет неверный токен"""
    response = await service_client.post("/tags/1/1")
    assert response.status == 401
