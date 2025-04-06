import pytest
from testsuite.databases import pgsql

async def test_update_nonexistent_note(service_client, auth_header):
    """Тест проверяет обновление несуществующей заметки"""
    response = await service_client.patch(
        "/notes/999",
        json={"title": "new"},
        headers=auth_header
    )
    assert response.status == 404


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_update_note_title(service_client, auth_header):
    """Тест проверяет обновление только заголовка заметки"""
    response = await service_client.patch(
        "/notes/1",
        json={"title": "new"},
        headers=auth_header
    )
    assert response.status == 200


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_update_note_body(service_client, auth_header):
    """Тест проверяет обновление только содержания заметки"""
    response = await service_client.patch(
        "/notes/1",
        json={"body": "new"},
        headers=auth_header
    )
    assert response.status == 200


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_update_note_title_and_body(service_client, auth_header):
    """Тест проверяет одновременное обновление заголовка и содержания заметки"""
    response = await service_client.patch(
        "/notes/1",
        json={"title": "new", "body": "new"},
        headers=auth_header
    )
    assert response.status == 200

async def test_update_note_invalid_token(service_client):
    """Тест проверяет невалидный токен"""
    response = await service_client.patch(
        "/notes/1",
        json={"title": "new", "body": "new"}    )
    assert response.status == 401

async def test_update_note_invalid_note_id(service_client, auth_header):
    """Тест проверяет невалидный айди заметки"""
    response = await service_client.patch(
        "/notes/-1",
        json={"title": "new", "body": "new"},
        headers=auth_header
    )
    assert response.status == 400

async def test_update_note_invalid_title(service_client, auth_header):
    """Тест проверяет невалидный заголовок"""
    response = await service_client.patch(
        "/notes/1",
        json={"title": "", "body": "new"},
        headers=auth_header
    )
    assert response.status == 400

