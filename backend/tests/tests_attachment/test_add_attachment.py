import pathlib
import pytest
from testsuite.databases import pgsql

async def test_add_attachment_note_doesnt_exist(service_client, form_data, auth_header):
    """Тест добавления вложения к несуществующей заметке"""
    response = await service_client.post('/attachment/999', data=form_data,
                                                            headers = auth_header)
    assert response.status == 404

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_add_attachment_success(service_client, form_data, auth_header):
    """Тест успешного добавления вложения"""    
    response = await service_client.post('/attachment/1', data=form_data,
                                                            headers = auth_header)
    assert response.status == 200
    assert response.content.decode() == '{"attchment_id":1}'
 