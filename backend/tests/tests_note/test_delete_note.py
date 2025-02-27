import pytest
from testsuite.databases import pgsql


# Нет заметки
async def test_delete_note_1(service_client, auth_header):
    response = await service_client.delete("/note/999",
                                         headers = auth_header)
    assert response.status == 404

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_delete_note_2(service_client, auth_header):
    response = await service_client.delete("/note/1",
                                          headers = auth_header)
    assert response.status == 200



