import pytest
from testsuite.databases import pgsql

@pytest.fixture
def auth_header():
    token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ0b2tlbl9leHBfdGltZSI6MTczODgzNzEwMiwidXNlcl9pZCI6MX0.SdRRnUsbmQeALGIejJI8Whf0KAqzBHLzTNcNw2LH9z0"
    headers = {
        "Authorization": f"Token {token}"
    }
    return headers


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_all_tags(service_client, auth_header):
    response = await service_client.get("/tags/all",
                                         headers = auth_header)
    expected = [
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




