import pytest
from testsuite.databases import pgsql

async def test_login_nonexistent_user(service_client):
    """Тест проверяет вход несуществующего пользователя, должен вернуть 404"""
    response = await service_client.post(
        "/auth/login",
        json={"username": "frubyl", "password": "frubasik"}
    )
    assert response.status == 404

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_login_wrong_password(service_client):
    """Тест проверяет вход с неправильным паролем, должен вернуть 401"""
    response = await service_client.post(
        "/auth/login",
        json={"username": "frubyl", "password": "frub"}
    )
    assert response.status == 401

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_login_successful(service_client):
    """Тест проверяет успешный вход с правильными данными, должен вернуть 200 и токен"""
    response = await service_client.post(
        "/auth/login",
        json={"username": "frubyl", "password": "frubasik"}
    )
    assert response.status == 200
    assert "access_token" in response.json()
    assert len(response.json()["access_token"]) > 0