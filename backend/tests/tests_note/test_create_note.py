import pytest
from testsuite.databases import pgsql

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_create_note(service_client, auth_header):
    """Тест проверяет успешное создание новой заметки"""
    response = await service_client.post("/notes",
                                         json={"title": "frubyl",
                                                 "body": "frubasik"}, 
                                        headers = auth_header)
    assert response.status == 201
    assert response.json()["note_id"] == 2

async def test_create_note_invalid_token(service_client):
    """Тест проверяет невалидгый токен"""
    response = await service_client.post("/notes",
                                         json={"title": "frubyl",
                                                 "body": "frubasik"})
    assert response.status == 401

async def test_create_note_invalid_title(service_client, auth_header):
    """Тест проверяет невалидный заголовок"""
    response = await service_client.post("/notes",
                                         json={"title": "",
                                                 "body": "frubasik"}, 
                                        headers = auth_header)
    assert response.status == 400


