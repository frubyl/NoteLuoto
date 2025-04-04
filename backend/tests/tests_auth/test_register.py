import pytest
from testsuite.databases import pgsql

async def test_register_new_user_success(service_client):
    """Тест проверяет успешную регистрацию нового пользователя"""
    test_user = {
        "username": "frubyl",
        "password": "frubasik789"
    }
    
    response = await service_client.post(
        "/auth/register",
        json=test_user
    )
    
    assert response.status == 201

async def test_register_invalid_password(service_client):
    """Тест проверяет регистрацию пользователя с неверным форматом пароля"""
    test_user = {
        "username": "user",
        "password": "123"
    }
    
    response = await service_client.post(
        "/auth/register",
        json=test_user
    )
    assert 'Invalid Password' in response.json()['message']
    assert response.status == 400


async def test_register_invalid_username(service_client):
    """Тест проверяет регистрацию пользователя с неверным форматом юзернейма"""
    test_user = {
        "username": "1",
        "password": "Password123"
    }
    
    response = await service_client.post(
        "/auth/register",
        json=test_user
    )
    assert 'Invalid Username' in response.json()['message']
    assert response.status == 400


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_register_existing_user_conflict(service_client):
    """Тест проверяет попытку регистрации с существующим именем пользователя"""
    existing_user = {
        "username": "frubyl", 
        "password": "frubasik123"
    }
    
    response = await service_client.post(
        "/auth/register",
        json=existing_user
    )
    
    assert response.status == 409

async def test_register_no_json(service_client):
    """Тест проверяет некорректный запрос"""

    response = await service_client.post(
        "/auth/register"
    )
    assert 'Invalid json format' in response.json()['message']
    assert response.status == 400