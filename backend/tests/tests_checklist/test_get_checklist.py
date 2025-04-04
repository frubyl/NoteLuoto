import pytest
from testsuite.databases import pgsql

async def test_get_nonexistent_checklist(service_client, auth_header):
    """Тест проверяет получение несуществующего чеклиста"""
    response = await service_client.get(
        "/checklist/99",
        headers=auth_header
    )
    assert response.status == 404


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_get_checklist_success(service_client, auth_header):
    """Тест проверяет успешное получение существующего чеклиста"""
    response = await service_client.get("/checklist/99",
                                         headers = auth_header)
    expected = {
    "title": "title",
    "created_at": "2025-03-10T09:30:00+00:00",
    "updated_at": "2025-03-10T09:30:00+00:00",
    "items": [
        {
        "item_id": 99,
        "text": "item1",
        "completed": False,
        "created_at": "2025-03-10T09:30:00+00:00",
        "updated_at": "2025-03-10T09:30:00+00:00"
        }
    ]
    }
    assert response.json() == expected
    assert response.status == 200


async def test_get_checklist_invalid_checklist_id(service_client, auth_header):
    """Тест проверяет невалидный айди чеклиста"""
    response = await service_client.get("/checklist/-1",
                                         headers = auth_header)

    assert response.status == 400

    

async def test_get_checklist_invalid_token(service_client):
    """Тест проверяет невалидный токен"""
    response = await service_client.get("/checklist/99")

    assert response.status == 401