import pathlib
import pytest
import aiohttp 
from testsuite.databases.pgsql import discover


import grpc
from samples import SyncService_pb2, SyncService_pb2_grpc
from samples import Langchain_pb2, Langchain_pb2_grpc
from samples import TagRecommender_pb2, TagRecommender_pb2_grpc


pytest_plugins = ['pytest_userver.plugins.postgresql', 'pytest_userver.plugins.core', 'pytest_userver.plugins.grpc']
@pytest.fixture
def form_data(load_binary):
    form_data = aiohttp.FormData()
    file = load_binary(pathlib.Path(__file__).parent / 'tests_attachment' / 'test_file.txt')
    form_data.add_field("file", file, filename='test_file.txt')
    return form_data

@pytest.fixture
def auth_header():
    token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ0b2tlbl9leHBfdGltZSI6MTc0MzcxNTcyMywidXNlcl9pZCI6MX0.B2cdOayFxAUZwexYwWwO6u0in1jY3l6G26nIftpnSr8"
    headers = {
        "Authorization": f"Bearer {token}"
    }
    return headers


@pytest.fixture(scope='session')
def service_source_dir():
    """Путь к корневой директории сервиса"""
    return pathlib.Path(__file__).parent.parent
    
@pytest.fixture(scope='session')
def initial_data_path(service_source_dir):
    """Путь к файлу с данными"""
    return [
        service_source_dir / 'postgresql/data',
    ]

@pytest.fixture(scope='session')
def pgsql_local(service_source_dir, pgsql_local_create):
    """Создание схемы базы данных для тестирования"""
    databases = discover.find_schemas(
        'noteluoto',  
        [service_source_dir.joinpath('postgresql/schemas')],
    )
    return pgsql_local_create(list(databases.values()))


