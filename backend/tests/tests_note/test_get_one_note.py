import pytest
from testsuite.databases import pgsql

async def test_get_nonexistent_note(service_client, auth_header):
    """Тест проверяет получение несуществующей заметки"""
    response = await service_client.get(
        "/notes/999",
        headers=auth_header
    )
    assert response.status == 404


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_get_existing_note(service_client, auth_header):
    """Тест проверяет успешное получение существующей заметки"""
    response = await service_client.get("/notes/1",
                                         headers = auth_header)
    assert response.status == 200
    assert response.json()["title"] == "title"
    assert response.json()["body"] == "body"
    assert response.json()["created_at"] == "2025-03-10T09:30:00+00:00"


async def test_get_invalid_token(service_client):
    """Тест проверяет невалидный токен"""
    response = await service_client.get(
        "/notes/999"    )
    assert response.status == 401

async def test_get_invalid_note_id(service_client, auth_header):
    """Тест проверяет невалидный айди заметки"""
    response = await service_client.get(
        "/notes/-1",
        headers = auth_header)

    assert response.status == 400