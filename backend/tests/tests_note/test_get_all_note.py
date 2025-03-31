import pytest
from testsuite.databases import pgsql

# Неавторизованный пользователь
async def test_get_all_notes_1(service_client, auth_header):
    response = await service_client.get("/notes")
    assert response.status == 401

# Проверка пагинации
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_get_all_notes_2(service_client, auth_header):
    response = await service_client.get("/notes",
                                        params = {"page":  "2",
                                                  "limit":  "2", 
                                                  "searchType": "none",
                                                  "tags": "tag3"},
                                        headers=auth_header)
    expected = {"total_count": 1, 
                 "notes": [
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

# ===========
# ТОЛЬКО ТЕГИ
# ===========

# Только по тегам - один тег- нет заметок 
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_get_all_notes_3(service_client, auth_header):
    response = await service_client.get("/notes",
                                        params = {"page":  "1",
                                                  "limit":  "20", 
                                                  "searchType": "none",
                                                  "tags": "doesntExist"},
                                        headers=auth_header)

    expected = {
                "total_count": 0, 
                "notes": []
    }
    assert response.status == 200
    assert response.json() == expected


# Только по тегам - один тег - есть заметки
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_get_all_notes_4(service_client, auth_header):
    response = await service_client.get("/notes",
                                        params = {"page":  "1",
                                                  "limit":  "20", 
                                                  "searchType": "none",
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

# Только по тегам - два тега - есть заметки
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_get_all_notes_5(service_client, auth_header):
    response = await service_client.get("/notes",
                                        params = {"page":  "1",
                                                  "limit":  "20", 
                                                  "searchType": "none",
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

# Только точный поиск - слово в заголовке
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_get_all_notes_6(service_client, auth_header):
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



# Только точный поиск - слово в теле
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_get_all_notes_7(service_client, auth_header):
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

# Только точный поиск - слово в заголовке чеклиста
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_get_all_notes_8(service_client, auth_header):
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

# Только точный поиск - слово в тексте пункта чеклиста 
@pytest.mark.pgsql('db_1', files=['initial_data.sql'])
async def test_get_all_notes_9(service_client, auth_header):
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
async def test_get_all_notes_10(service_client, auth_header):
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

# # Только семантический поиск
# async def test_get_all_notes_11(service_client, auth_header, grpc_mockserver):
#     # Мок сервер для grpc
#     @grpc_mockserver('LangChain.SemanticSearch')
#     async def mock_search(request, context):
#         return SemanticSearchResult(notes_id=[4, 5, 6])

#     response = await service_client.get("/notes",
#                                         params = {"page":  "1",
#                                                   "limit":  "20", 
#                                                   "searchType": "semantic",
#                                                   "query": "query"},
#                                         headers=auth_header)
#     expected = [
#                 {
#                     "note_id": 4,
#                     "title": "title4",
#                     "body": "body4",
#                     "created_at": "2025-03-10T09:00:00+00:00",
#                     "updated_at": "2025-03-10T09:00:00+00:00"
#                 },
#                 {
#                     "note_id": 5,
#                     "title": "title",
#                     "body": "body",
#                     "created_at": "2025-03-10T08:00:00+00:00",
#                     "updated_at": "2025-03-10T08:00:00+00:00"
#                 },
#                 {
#                     "note_id": 6,
#                     "title": "title",
#                     "body": "body",
#                     "created_at": "2025-03-10T07:00:00+00:00",
#                     "updated_at": "2025-03-10T07:00:00+00:00"
#                 }
#     ]
#     assert response.status == 200
#     assert response.json() == expected

# Семантический + теги - нужна заглушка 
# async def test_get_all_notes_1(service_client, auth_header):
#     response = await service_client.get("/notes")
#     assert response.status == 404


