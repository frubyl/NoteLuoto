import pytest
from testsuite.databases import pgsql

async def test_delete_nonexistent_note(service_client, auth_header):
    """Тест проверяет удаление несуществующей заметки"""
    response = await service_client.delete(
        "/note/999",
        headers=auth_header
    )
    assert response.status == 404

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_delete_note_success(service_client, auth_header):
    """Тест проверяет успешное удаление существующей заметки"""
    response = await service_client.delete(
        "/note/1", 
        headers=auth_header
    )
    assert response.status == 200
