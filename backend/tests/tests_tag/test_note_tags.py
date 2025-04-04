import pytest
from testsuite.databases import pgsql


async def test_get_note_tags_note_doesnt_exist(service_client, auth_header):
    """Тест проверяет получение тегов у несуществующей заметки"""
    response = await service_client.get("/tags/1",
                                         headers = auth_header)
    assert response.status == 404


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_get_note_tags_success(service_client, auth_header):
    """Тест проверяет успешное получение тегов"""
    response = await service_client.get("/tags/1",
                                         headers = auth_header)
    expected = [
        {
            "tag_id": 2,
            "name": "tag2"
        }
    ]
    assert response.status == 200
    assert response.json() == expected


async def test_get_note_invalid_request(service_client, auth_header):
    """Тест проверяет неверный запрос"""
    response = await service_client.get("/tags/-1",
                                         headers = auth_header)
    assert response.status == 400

async def test_get_note_invalid_token(service_client):
    """Тест проверяет невалидный токен"""
    response = await service_client.get("/tags/1")
    assert response.status == 401
