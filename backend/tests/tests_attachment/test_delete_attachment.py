import pytest
from testsuite.databases import pgsql

async def test_delete_attachment_that_doesnt_exist(service_client, auth_header):
    """Тест удаление несуществующего вложения"""
    response = await service_client.delete('/attachment/999', headers = auth_header)
    assert response.status == 404

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_delete_attachment_file_on_server_doesnt_exist(service_client, auth_header):
    """Тест удаление вложения файл которого на сервере отсутствует"""
    response = await service_client.delete('/attachment/22', headers = auth_header)
    assert response.status == 500

# для проверки надо создать файл del.txt в build_*/attachments/
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_delete_attachment_success(service_client, auth_header, create_del_txt):
    """Тест успешное удаление"""
    response = await service_client.delete('/attachment/20', headers = auth_header)
    assert response.status == 200

async def test_delete_attachment_invalid_attachment_id(service_client, auth_header):
    """Тест неверные данные запроса"""
    response = await service_client.delete('/attachment/-1', headers = auth_header)
    assert response.status == 400

async def test_delete_attachment_file_invalid_token(service_client):
    """Тест неверный токен"""
    response = await service_client.delete('/attachment/22')
    assert response.status == 401