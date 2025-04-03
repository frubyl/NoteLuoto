import pytest
from testsuite.databases import pgsql

async def test_register_new_user_success(service_client):
    """Тест проверяет успешную регистрацию нового пользователя"""
    test_user = {
        "username": "new_test_user",
        "password": "test_password123"
    }
    
    response = await service_client.post(
        "/auth/register",
        json=test_user
    )
    
    assert response.status == 201


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_register_existing_user_conflict(service_client):
    """Тест проверяет попытку регистрации с существующим именем пользователя"""
    existing_user = {
        "username": "frubyl", 
        "password": "frubasik"
    }
    
    response = await service_client.post(
        "/auth/register",
        json=existing_user
    )
    
    assert response.status == 409
