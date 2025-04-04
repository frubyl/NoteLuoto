import pytest
from testsuite.databases import pgsql

async def test_note_delete_tag_note_doesnt_exist(service_client, auth_header):
    """Тест проверяет удаление тега у несуществующей заметке"""
    response = await service_client.delete("/tags/99/99",
                                         headers = auth_header)
    assert response.status == 404

async def test_note_delete_tag_doesnt_exist(service_client, auth_header):
    """Тест проверяет удаление несуществующего тега"""
    response = await service_client.delete("/tags/99/99",
                                         headers = auth_header)
    assert response.status == 404

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_note_delete_tag_wasnt_add_to_note(service_client, auth_header):
    """Тест проверяет удаление тега который не был добавлен к заметке"""
    response = await service_client.delete("/tags/1/1",
                                         headers = auth_header)
    assert response.status == 404

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_note_delete_tag_success(service_client, auth_header):
    """Тест проверяет успешное удаление тега"""
    response = await service_client.delete("/tags/1/2",
                                         headers = auth_header)
    assert response.status == 200

async def test_note_delete_tag_invalid_note_id(service_client, auth_header):
    """Тест проверяет неверный айди заметки"""
    response = await service_client.delete("/tags/-1/99",
                                         headers = auth_header)
    assert response.status == 400

async def test_note_delete_tag_invalid_tag_id(service_client, auth_header):
    """Тест проверяет неверный айди тега"""
    response = await service_client.delete("/tags/99/-1",
                                            headers = auth_header)
    assert response.status == 400

async def test_note_delete_tag_invalid_token(service_client):
    """Тест проверяет неверный айди тега"""
    response = await service_client.delete("/tags/99/1")
    assert response.status == 401