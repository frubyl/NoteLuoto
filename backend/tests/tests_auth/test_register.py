import pytest
from testsuite.databases import pgsql

# Тестирование регистрации нового пользователя
async def test_register_1(service_client):
    response = await service_client.post("/auth/register",
                                         json={"username": "frubyl",
                                                 "password": "frubasik"})
    assert response.status == 201

# Тестирование регистрации пользователя, когда существует другой пользователь с таким же username
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_register_2(service_client):
    response = await service_client.post("/auth/register",
                                         json={"username": "frubyl",
                                                 "password": "frubasik"})
    assert response.status == 409
