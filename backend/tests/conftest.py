import pathlib
import pytest
import aiohttp
from testsuite.databases.pgsql import discover
import grpc
import samples.Langchain_pb2_grpc as langchain_services
import samples.TagRecommender_pb2_grpc as tag_recommender_services
import os

USERVER_CONFIG_HOOKS = ['prepare_service_config']

pytest_plugins = [
    'pytest_userver.plugins.postgresql',
    'pytest_userver.plugins.core',
    'pytest_userver.plugins.grpc'
]


# МОКИРОВАНИЕ LangChain grpc клиента
@pytest.fixture(scope='session')
def mock_grpc_langchain_session(grpc_mockserver, create_grpc_mock):
    """Фикстура для мокирования LangChain gRPC сервиса"""
    mock = create_grpc_mock(langchain_services.LangChainServicer)
    langchain_services.add_LangChainServicer_to_server(
        mock.servicer, grpc_mockserver,
    )
    return mock

@pytest.fixture
def mock_grpc_langchain(mock_grpc_langchain_session):
    """Фикстура для мока на уровне теста"""
    with mock_grpc_langchain_session.mock() as mock:
        yield mock
 
 
# МОКИРОВАНИЕ TagRecommender grpc клиента
@pytest.fixture(scope='session')
def mock_grpc_tag_recommender_session(grpc_mockserver, create_grpc_mock):
    """Фикстура для мокирования TagRecommender gRPC сервиса"""
    mock = create_grpc_mock(tag_recommender_services.TagRecommenderServicer)
    tag_recommender_services.add_TagRecommenderServicer_to_server(
        mock.servicer, grpc_mockserver,
    )
    return mock

@pytest.fixture
def mock_grpc_tag_recommender(mock_grpc_tag_recommender_session):
    """Фикстура для мока на уровне теста"""
    with mock_grpc_tag_recommender_session.mock() as mock:
        yield mock



# Настройки тестового окружения
@pytest.fixture(scope='session')
def prepare_service_config(grpc_mockserver_endpoint):
    """Подмена endpoint в конфиге сервиса"""
    def patch_config(config, config_vars):
        components = config['components_manager']['components']
        components['langchain-client']['endpoint'] = grpc_mockserver_endpoint
        components['tag-recommender-client']['endpoint'] = grpc_mockserver_endpoint
    return patch_config

@pytest.fixture
def form_data(load_binary):
    """Фикстура для создания form-data"""
    form_data = aiohttp.FormData()
    file = load_binary(pathlib.Path(__file__).parent / 'tests_attachment' / 'test_file.txt')
    form_data.add_field("file", file, filename='test_file.txt')
    return form_data

@pytest.fixture
def auth_header():
    """Фикстура для заголовков авторизации"""
    token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ0b2tlbl9leHBfdGltZSI6MTc0NTU5MzA0MiwidXNlcl9pZCI6MX0.aG6PuSnCI0XfqqmhHrEb9ezwFAEVr52v3K4rzHSWjrw"
    return {"Authentication": f"Bearer {token}"}  

@pytest.fixture(scope='session')
def service_source_dir():
    """Путь к корневой директории сервиса"""
    return pathlib.Path(__file__).parent.parent

@pytest.fixture(scope='session')
def initial_data_path(service_source_dir):
    """Путь к файлу с данными"""
    return [service_source_dir / 'postgresql/data']

@pytest.fixture(scope='session')
def pgsql_local(service_source_dir, pgsql_local_create):
    """Создание схемы базы данных для тестирования"""
    databases = discover.find_schemas(
        'noteluoto',
        [service_source_dir.joinpath('postgresql/schemas')],
    )
    return pgsql_local_create(list(databases.values()))


@pytest.fixture
def create_del_txt(service_source_dir):
    build_dir1 = service_source_dir / 'build_release' / 'attachments'
    build_dir2 = service_source_dir / 'build_debug' / 'attachments'

    file_path1 = build_dir1 / 'del.txt'
    file_path2 = build_dir2 / 'del.txt'
    if build_dir1.exists():
        file_path1.touch()

    if build_dir2.exists():
        file_path2.touch()

@pytest.fixture
def create_get_txt(service_source_dir):
    file_content = "Hello, this is a new file created in C++!\n"
    build_dirs = [
        service_source_dir / 'build_release' / 'attachments',
        service_source_dir / 'build_debug' / 'attachments'
    ]
    
    
    for build_dir in build_dirs:
        if build_dir.exists():
            file_path = build_dir / 'test.txt'
            
            with open(file_path, 'w') as f:
                f.write(file_content)

    

