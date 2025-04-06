import pytest
from testsuite.databases import pgsql

async def test_delete_nonexistent_note(service_client, auth_header):
    """Тест проверяет удаление несуществующей заметки"""
    response = await service_client.delete(
        "/notes/999",
        headers=auth_header
    )
    assert response.status == 404

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_delete_note_success(service_client, auth_header):
    """Тест проверяет успешное удаление существующей заметки"""
    response = await service_client.delete(
        "/notes/1", 
        headers=auth_header
    )
    assert response.status == 200

async def test_delete_note_invalid_note_id(service_client, auth_header):
    """Тест проверяет неверное значение note_id"""
    response = await service_client.delete(
        "/notes/-1", 
        headers=auth_header
    )
    assert response.status == 400

async def test_delete_note_invalid_token(service_client):
    """Тест проверяет неверный токен"""
    response = await service_client.delete(
        "/notes/1"
    )
    assert response.status == 401
