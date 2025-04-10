import pytest
from testsuite.databases import pgsql

async def test_get_nonexistent_attachment(service_client, auth_header):
    """Тест проверяет получение несуществующего вложения, должен вернуть 404"""
    response = await service_client.get('/attachment/999', headers=auth_header)
    assert response.status == 404

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_get_attachment_with_missing_file(service_client, auth_header):
    """Тест проверяет получение вложения с отсутствующим файлом, должен вернуть 500"""
    response = await service_client.get('/attachment/22', headers=auth_header)
    assert response.status == 500


# для проверки надо создать файл test.txt в build_*/attachments/
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_get_attachment_successfully(service_client, auth_header, create_get_txt):
    """Тест проверяет успешное получение существующего вложения"""
    response = await service_client.get('/attachment/23', headers=auth_header)
    expected = {
        "file_name": "test.txt",
        "content": "Hello, this is a new file created in C++!\n"
    }
    assert response.status == 200
    assert response.json() == expected


async def test_get_attachment_invalid_request(service_client, auth_header):
    """Тест проверяет неверный запрос"""
    response = await service_client.get('/attachment/-1', headers=auth_header)
    assert response.status == 400

async def test_get_attachment_invalid_token(service_client, auth_header):
    """Тест проверяет успешное получение существующего вложения"""
    response = await service_client.get('/attachment/23')

    assert response.status == 401
