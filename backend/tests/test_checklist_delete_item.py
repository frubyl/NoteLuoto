import pytest
from testsuite.databases import pgsql

@pytest.fixture
def auth_header():
    token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ0b2tlbl9leHBfdGltZSI6MTczODgzNzEwMiwidXNlcl9pZCI6MX0.SdRRnUsbmQeALGIejJI8Whf0KAqzBHLzTNcNw2LH9z0"
    headers = {
        "Authorization": f"Token {token}"
    }
    return headers

# Нет айтема
async def test_checklist_delete_item_1(service_client, auth_header):
    response = await service_client.delete("/checklist/item/1",
                                         headers = auth_header)
    assert response.status == 404

# Все ок
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_checklist_delete_item_2(service_client, auth_header):
    response = await service_client.delete("/checklist/item/1",
                                         headers = auth_header)
    assert response.status == 200

    
