import pytest
from testsuite.databases import pgsql


# Тег уже существует
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_create_tag_1(service_client, auth_header):
    response = await service_client.post("/tags/create",
                                            json = {"name": "tag1"},
                                         headers = auth_header)
    assert response.status == 409

# Новый тег
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_create_tag_2(service_client, auth_header):
    response = await service_client.post("/tags/create",
                                            json = {"name": "createtag"},
                                          headers = auth_header)
    assert response.status == 201
    assert response.json()["tag_id"] == 3




