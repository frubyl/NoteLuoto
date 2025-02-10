import pytest
from testsuite.databases import pgsql


# Нет заметки
async def test_note_tags_1(service_client, auth_header):
    response = await service_client.get("/tags/1",
                                         headers = auth_header)
    assert response.status == 404


# Получение всех тегов
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_note_tags_2(service_client, auth_header):
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
