import pytest
from testsuite.databases import pgsql
import samples.Langchain_pb2 as langchain_protos

async def test_unauthorized_access_to_notes(service_client, auth_header):
    """Тест проверяет доступ к заметкам без авторизации"""
    response = await service_client.get("/notes")
    assert response.status == 401



async def test_notes_invalid_page(service_client, auth_header):
    """Тест проверяет некорректную страницу"""
    response = await service_client.get("/notes",
                                        params = {"page":  "-2",
                                                  "limit":  "2", 
                                                  "tags": "tag3"},
                                        headers=auth_header)

    assert response.status == 400

async def test_notes_invalid_limit(service_client, auth_header):
    """Тест проверяет некорректный лимит"""
    response = await service_client.get("/notes",
                                        params = {"page":  "2",
                                                  "limit":  "-2", 
                                                  "tags": "tag3"},
                                        headers=auth_header)

    assert response.status == 400

async def test_notes_invalid_searchType(service_client, auth_header):
    """Тест проверяет некорректный тип поиска"""
    response = await service_client.get("/notes",
                                        params = {"page":  "2",
                                                  "limit":  "-2", 
                                                  "searchType": "t",
                                                  "tags": "tag3"},
                                        headers=auth_header)

    assert response.status == 400

async def test_notes_invalid_query(service_client, auth_header):
    """Тест проверяет некорректный запрос"""
    response = await service_client.get("/notes",
                                        params = {"page":  "2",
                                                  "query":  "", 
                                                  "tags": "tag3"},
                                        headers=auth_header)

    assert response.status == 400

async def test_notes_invalid_tags(service_client, auth_header):
    """Тест проверяет некорректный массив тегов"""
    response = await service_client.get("/notes",
                                        params = {"page":  "2",
                                                  "tags": ["1", "1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1"]},
                                        headers=auth_header)

    assert response.status == 400

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_notes_pagination(service_client, auth_header):
    """Тест проверяет корректность пагинации при получении списка заметок"""
    response = await service_client.get("/notes",
                                        params = {"page":  "2",
                                                  "limit":  "5", 
                                                  "tags": "tag3"},
                                        headers=auth_header)
    expected = {"total_count":0, 
                 "notes": [
          
                ]
    }
    assert response.status == 200
    assert response.json() == expected
# ===========
# ТОЛЬКО ТЕГИ
# ===========

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_tag_filter_no_results(service_client, auth_header):
    """Тест проверяет фильтрацию по несуществующему тегу"""
    response = await service_client.get("/notes",
                                        params = {"page":  "1",
                                                  "limit":  "20", 
                                                  "tags": "doesntExist"},
                                        headers=auth_header)

    expected = {
                "total_count": 0, 
                "notes": []
    }
    assert response.status == 200
    assert response.json() == expected


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_single_tag_filter(service_client, auth_header):
    """Тест проверяет фильтрацию по одному существующему тегу"""
    response = await service_client.get("/notes",
                                        params = {"page":  "1",
                                                  "limit":  "20", 
                                                  "tags": "tag3"},
                                        headers=auth_header)
    expected = {
                "total_count": 3, 
                "notes": [
                {
                    "note_id": 4,
                    "title": "title4",
                    "body": "body4",
                    "created_at": "2025-03-10T09:00:00+00:00",
                    "updated_at": "2025-03-10T09:00:00+00:00"
                },
                {
                    "note_id": 5,
                    "title": "title",
                    "body": "body",
                    "created_at": "2025-03-10T08:00:00+00:00",
                    "updated_at": "2025-03-10T08:00:00+00:00"
                },
                {
                    "note_id": 6,
                    "title": "title",
                    "body": "body",
                    "created_at": "2025-03-10T07:00:00+00:00",
                    "updated_at": "2025-03-10T07:00:00+00:00"
                }
                ]
    }
    assert response.status == 200
    assert response.json() == expected


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_multiple_tags_filter(service_client, auth_header):
    """Тест проверяет фильтрацию по нескольким тегам одновременно"""
    response = await service_client.get("/notes",
                                        params = {"page":  "1",
                                                  "limit":  "20", 
                                                  "tags": ["tag3", "tag2"]},
                                        headers=auth_header)
    expected = {
                "total_count" : 1,
                "notes": [
                {
                    "note_id": 4,
                    "title": "title4",
                    "body": "body4",
                    "created_at": "2025-03-10T09:00:00+00:00",
                    "updated_at": "2025-03-10T09:00:00+00:00"
                }
                ]
    }
    assert response.status == 200
    assert response.json() == expected



# ============
# ТОЧНЫЙ ПОИСК 
# ============

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_exact_search_russian(service_client, auth_header):
    """Тест проверяет точный поиск на русском языке"""    
    response = await service_client.get("/notes",
                                        params = {"page":  "1",
                                                  "limit":  "20", 
                                                  "searchType": "exact",
                                                  "query": "заголовок"},
                                        headers=auth_header)
    expected = {
                "total_count" : 1,
                "notes": [
                {
                      
                    "note_id": 7,
                    "title": "заголовок",
                    "body": "body",
                    "created_at": "2025-03-10T07:00:00+00:00",
                    "updated_at": "2025-03-10T07:00:00+00:00"
                
                }
                ]
    }
    assert response.status == 200
    assert response.json() == expected


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_exact_search_title(service_client, auth_header):
    """Тест проверяет точный поиск по заголовку заметки"""    
    response = await service_client.get("/notes",
                                        params = {"page":  "1",
                                                  "limit":  "20", 
                                                  "searchType": "exact",
                                                  "query": "title4"},
                                        headers=auth_header)
    expected = {
                "total_count" : 1,
                "notes": [
                {
                    "note_id": 4,
                    "title": "title4",
                    "body": "body4",
                    "created_at": "2025-03-10T09:00:00+00:00",
                    "updated_at": "2025-03-10T09:00:00+00:00"
                }
                ]
    }
    assert response.status == 200
    assert response.json() == expected



@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_exact_search_body(service_client, auth_header):
    """Тест проверяет точный поиск по содержимому заметки"""    
    response = await service_client.get("/notes",
                                        params = {"page":  "1",
                                                  "limit":  "20", 
                                                  "searchType": "exact",
                                                  "query": "body4"},
                                        headers=auth_header)
    expected = {
                "total_count" : 1,
                "notes": [
                {
                    "note_id": 4,
                    "title": "title4",
                    "body": "body4",
                    "created_at": "2025-03-10T09:00:00+00:00",
                    "updated_at": "2025-03-10T09:00:00+00:00"
                }]
    }
    assert response.status == 200
    assert response.json() == expected

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_exact_search_checklist_title(service_client, auth_header):
    """Тест проверяет точный поиск по заголовку чеклиста"""   
    response = await service_client.get("/notes",
                                        params = {"page":  "1",
                                                  "limit":  "20", 
                                                  "searchType": "exact",
                                                  "query": "checklistTitle"},
                                        headers=auth_header)
    expected = {
                "total_count" : 1,
                "notes": [
                {
                    "note_id": 4,
                    "title": "title4",
                    "body": "body4",
                    "created_at": "2025-03-10T09:00:00+00:00",
                    "updated_at": "2025-03-10T09:00:00+00:00"
                }
                ]
    }
    assert response.status == 200
    assert response.json() == expected

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_exact_search_checklist_item(service_client, auth_header):
    """Тест проверяет точный поиск по пункту чеклиста"""    
    response = await service_client.get("/notes",
                                        params = {"page":  "1",
                                                  "limit":  "20", 
                                                  "searchType": "exact",
                                                  "query": "checklistItem1"},
                                        headers=auth_header)
    expected = {
                "total_count" : 1,
                "notes": [
                {
                    "note_id": 4,
                    "title": "title4",
                    "body": "body4",
                    "created_at": "2025-03-10T09:00:00+00:00",
                    "updated_at": "2025-03-10T09:00:00+00:00"
                }
                ]
    }
    assert response.status == 200
    assert response.json() == expected


# =============
# ТОЧНЫЙ + ТЕГИ
# =============

@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_combined_exact_search_with_tags(service_client, auth_header):
    """Тест проверяет комбинированный поиск (точный + теги)"""    
    response = await service_client.get("/notes",
                                        params = {"page":  "1",
                                                  "limit":  "20", 
                                                  "searchType": "exact",
                                                  "tags": "tag3", 
                                                  "query": "checklistItem1"},
                                        headers=auth_header)
    expected = {
                "total_count" : 1,
                "notes": [
                {
                    "note_id": 4,
                    "title": "title4",
                    "body": "body4",
                    "created_at": "2025-03-10T09:00:00+00:00",
                    "updated_at": "2025-03-10T09:00:00+00:00"
                }
                ]
    }
    assert response.status == 200
    assert response.json() == expected


# =============
# СЕМАНТИЧЕСКИЙ
# =============
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_semantic_search(service_client, auth_header, mock_grpc_langchain):
    """Тест проверяет только семантический поиск"""
    @mock_grpc_langchain('SemanticSearch')
    async def mock_recommend_prompts(request, context):
        return langchain_protos.SemanticSearchResult(
            notes_id=[4, 5, 6]
        )

    response = await service_client.get("/notes",
                                        params = {"page":  "1",
                                                  "limit":  "20", 
                                                  "searchType": "semantic",
                                                  "query": "query"},
                                        headers=auth_header)
    expected = {
                "total_count" : 3,
                "notes": [
                {
                    "note_id": 4,
                    "title": "title4",
                    "body": "body4",
                    "created_at": "2025-03-10T09:00:00+00:00",
                    "updated_at": "2025-03-10T09:00:00+00:00"
                },
                {
                    "note_id": 5,
                    "title": "title",
                    "body": "body",
                    "created_at": "2025-03-10T08:00:00+00:00",
                    "updated_at": "2025-03-10T08:00:00+00:00"
                },
                {
                    "note_id": 6,
                    "title": "title",
                    "body": "body",
                    "created_at": "2025-03-10T07:00:00+00:00",
                    "updated_at": "2025-03-10T07:00:00+00:00"
                }
                ]
    }
    assert response.status == 200
    assert response.json() == expected


# ====================
# СЕМАНТИЧЕСКИЙ + ТЕГИ
# ====================
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_semantic_search_with_tags(service_client, auth_header, mock_grpc_langchain):
    """Тест проверяет комбинированный поиск (семантический + теги)"""    
    @mock_grpc_langchain('SemanticSearch')
    async def mock_recommend_prompts(request, context):
        return langchain_protos.SemanticSearchResult(
            notes_id=[4, 5, 6]
        )

    response = await service_client.get("/notes",
                                        params = {"page":  "1",
                                                  "limit":  "20", 
                                                  "tags": "tag2", 
                                                  "searchType": "semantic",
                                                  "query": "query"},
                                        headers=auth_header)
    expected = {
                "total_count" : 1,
                "notes": [
                {
                    "note_id": 4,
                    "title": "title4",
                    "body": "body4",
                    "created_at": "2025-03-10T09:00:00+00:00",
                    "updated_at": "2025-03-10T09:00:00+00:00"
                }
                ]
    }
    assert response.status == 200
    assert response.json() == expected


@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_semantic_search_grpc_error(service_client, auth_header, mock_grpc_langchain):
    """Тест ошибки gRPC сервиса"""
    @mock_grpc_langchain('SemanticSearch')
    async def mock_recommend_prompts(request, context):
        raise _AbortionError(StatusCode.UNAVAILABLE, "Service unavailable")

    response = await service_client.get("/notes",
                                        params = {"page":  "1",
                                                  "limit":  "20", 
                                                  "tags": "tag3", 
                                                  "searchType": "semantic",
                                                  "query": "query"},
                                        headers=auth_header)
    assert response.status == 500
    assert 'Internal Server Error' in response.json()['message']


