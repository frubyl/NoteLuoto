import pytest
from testsuite.databases import pgsql

async def test_update_nonexistent_note(service_client, auth_header):
    """Тест проверяет обновление несуществующей заметки"""
    response = await service_client.patch(
        "/note/999",
        json={"title": "new"},
        headers=auth_header
    )
    assert response.status == 404


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_update_note_title(service_client, auth_header):
    """Тест проверяет обновление только заголовка заметки"""
    response = await service_client.patch(
        "/note/1",
        json={"title": "new"},
        headers=auth_header
    )
    assert response.status == 200


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_update_note_body(service_client, auth_header):
    """Тест проверяет обновление только содержания заметки"""
    response = await service_client.patch(
        "/note/1",
        json={"body": "new"},
        headers=auth_header
    )
    assert response.status == 200


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_update_note_title_and_body(service_client, auth_header):
    """Тест проверяет одновременное обновление заголовка и содержания заметки"""
    response = await service_client.patch(
        "/note/1",
        json={"title": "new", "body": "new"},
        headers=auth_header
    )
    assert response.status == 200
