import pytest
from testsuite.databases import pgsql

async def test_add_checklist_to_nonexistent_note(service_client, auth_header):
    """Тест проверяет добавление чеклиста к несуществующей заметке"""
    response = await service_client.post(
        "/checklist/99",
        json = {"title": "title"},
        headers=auth_header
    )

    assert response.status == 404


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_add_checklist_to_note_success(service_client, auth_header):
    """Тест проверяет успешное добавление чеклиста к заметке"""
    response = await service_client.post(
        "/checklist/1",
        json = {"title": "title"},
        headers = auth_header)

    assert response.status == 201
    assert response.json()["checklist_id"] == 3

async def test_add_checklist_to_note_invalid_note_id(service_client, auth_header):
    """Тест проверяет невалидный айди заметки"""
    response = await service_client.post(
        "/checklist/-1",
        json = {"title": "title"},
        headers = auth_header)

    assert response.status == 400

async def test_add_checklist_to_note_invalid_title(service_client, auth_header):
    """Тест проверяет невалидный заголовок"""
    response = await service_client.post(
        "/checklist/1",
        json = {"title": ""},
        headers = auth_header)

    assert response.status == 400

async def test_add_checklist_to_note_invalid_token(service_client):
    """Тест проверяет невалидный токен"""
    response = await service_client.post(
        "/checklist/1",
        json = {"title": "sdf"})

    assert response.status == 401

