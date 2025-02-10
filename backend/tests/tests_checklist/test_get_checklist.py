import pytest
from testsuite.databases import pgsql

# Нет заметки
async def test_get_checklist_1(service_client, auth_header):
    response = await service_client.get("/checklist/99",
                                         headers = auth_header)
    assert response.status == 404

# Все ок
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_get_checklist_2(service_client, auth_header):
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

    
