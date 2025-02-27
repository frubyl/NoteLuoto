import pytest
from testsuite.databases import pgsql

async def test_get_attachment_1(service_client, auth_header):
    response = await service_client.get('/attachment/999', headers = auth_header)
    assert response.status == 404

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_get_attachment_2(service_client, auth_header):
    response = await service_client.get('/attachment/22', headers = auth_header)
    assert response.status == 500

# для проверки надо создать файл test.txt в build_*/attachments/
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_get_attachment_3(service_client, auth_header):
    response = await service_client.get('/attachment/23', headers = auth_header)
    expected = {"file_name":"test.txt",
                "content":"Hello, this is a new file created in C++!\n"}
    assert response.status == 200
    assert response.json() == expected
