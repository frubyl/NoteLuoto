import pathlib
import pytest

from testsuite.databases.pgsql import discover


pytest_plugins = ['pytest_userver.plugins.postgresql']

@pytest.fixture
def auth_header():
    token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ0b2tlbl9leHBfdGltZSI6MTczOTc4ODg3OSwidXNlcl9pZCI6MX0.DBL11QQC4Nvl1y-d09nYF3y5aJkJwtdFPfMEw5CDeIo"
    headers = {
        "Authorization": f"Token {token}"
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

