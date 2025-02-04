import pytest
from testsuite.databases import pgsql

# Тестирование входа пользователя, которого нет в базе данных
async def test_login_1(service_client):
    response = await service_client.post("/auth/login",
                                         json={"username": "frubyl",
                                                 "password": "frubasik"})
    assert response.status == 404

# Тестирование входа пользователя, пароли не совпадают
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_login_2(service_client):
    response = await service_client.post("/auth/login",
                                         json={"username": "frubyl",
                                                 "password": "frub"})
    assert response.status == 401

# Тестирование входа пользователя, пароли совпадают
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_login_3(service_client):
    response = await service_client.post("/auth/login",
                                         json={"username": "frubyl",
                                                 "password": "frubasik"})
    assert response.status == 200
    assert response.json()["access_token"] != ""
