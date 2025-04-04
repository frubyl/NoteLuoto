import pytest
from testsuite.databases import pgsql


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_create_tag_already_exist(service_client, auth_header):
    """Тест проверяет создание тега который уже существует"""
    response = await service_client.post("/tags/create",
                                            json = {"name": "tag1"},
                                         headers = auth_header)
    assert response.status == 409

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_create_tag_success(service_client, auth_header):
    """Тест проверяет успешное создание тега"""
    response = await service_client.post("/tags/create",
                                            json = {"name": "createtag"},
                                          headers = auth_header)
    assert response.status == 201
    assert response.json()["tag_id"] == 4


async def test_create_tag_empty(service_client, auth_header):
    """Тест проверяет успешное создание тега"""
    response = await service_client.post("/tags/create",
                                            json = {"name": ""},
                                          headers = auth_header)
    assert response.status == 400


async def test_create_tag_empty_max_lenght_exeded(service_client, auth_header):
    """Тест проверяет успешное создание тега"""
    response = await service_client.post("/tags/create",
                                            json = {"name": "fsdgdfjgidfghdfughdsdfgsdfdfgdfggsdfgsifghdfgdsfgsdfg"},
                                          headers = auth_header)
    assert response.status == 400

async def test_create_tag_empty_no_json(service_client, auth_header):
    """Тест проверяет успешное создание тега"""
    response = await service_client.post("/tags/create",
                                          headers = auth_header)
    assert response.status == 400

async def test_create_tag_empty_invalid_token(service_client):
    """Тест проверяет успешное создание тега"""
    response = await service_client.post("/tags/create",
                                            json = {"name": "fsdgdfjgidfghdfughdsdfgsdfdfgdfggsdfgsifghdfgdsfgsdfg"})
    assert response.status == 401
