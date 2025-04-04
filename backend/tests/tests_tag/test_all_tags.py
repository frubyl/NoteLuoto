import pytest
from testsuite.databases import pgsql


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_all_tags(service_client, auth_header):
    """Тест проверяет получение всех тегов которые существует"""
    response = await service_client.get("/tags/all",
                                         headers = auth_header)
    expected = [
        {
            "tag_id": 3,
            "name": "tag3"
        },
        {
            "tag_id": 2,
            "name": "tag2"
        },
        {
            "tag_id": 1,
            "name": "tag1"
        }
    ]
    assert response.status == 200
    assert response.json() == expected


async def test_all_tags_empty_answer(service_client, auth_header):
    """Тест проверяет получение пустого ответа"""
    response = await service_client.get("/tags/all",
                                         headers = auth_header)
    expected = [
    ]
    assert response.status == 200
    assert response.json() == expected


async def test_all_tags_invalid_token(service_client):
    """Тест невалидный токен"""
    response = await service_client.get("/tags/all")

    assert response.status == 401
