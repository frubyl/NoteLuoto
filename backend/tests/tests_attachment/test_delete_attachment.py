import pytest
from testsuite.databases import pgsql

async def test_delete_attachment_1(service_client, auth_header):
    response = await service_client.delete('/attachment/999', headers = auth_header)
    assert response.status == 404

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_delete_attachment_2(service_client, auth_header):
    response = await service_client.delete('/attachment/22', headers = auth_header)
    assert response.status == 500

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_delete_attachment_3(service_client, auth_header):
    response = await service_client.delete('/attachment/20', headers = auth_header)

    assert response.status == 200
