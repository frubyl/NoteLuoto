cmake_minimum_required(VERSION 3.12)
project(noteluoto CXX)

# Подключение стандартных директорий установки 
include(GNUInstallDirs)

# ================================
# ДОБАВЛЕНИЕ ЗАВИСИМОСТЕЙ ЮСЕРВЕРА 
# ================================

find_package(userver COMPONENTS core grpc postgresql QUIET)
if(NOT userver_FOUND)  #
    # Включить библиотеки userver, необходимые в этом проекте
    set(USERVER_FEATURE_POSTGRESQL ON CACHE BOOL "" FORCE)
    set(USERVER_FEATURE_GRPC ON CACHE BOOL "" FORCE)

    set(USERVER_FEATURE_CRYPTOPP_BLAKE2 OFF CACHE BOOL "" FORCE)
    set(USERVER_FEATURE_GRPC_CHANNELZ OFF CACHE BOOL "" FORCE)
    set(USERVER_FEATURE_REDIS_HI_MALLOC ON CACHE BOOL "" FORCE)

    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/userver)
        message(STATUS "Using userver framework from third_party/userver")
        add_subdirectory(third_party/userver)
    else()
        message(FATAL_ERROR "Either install the userver or provide a path to it")
    endif()
endif()

userver_setup_environment()

# ===========================
# ДОБАВЛЕНИЕ ФАЙЛОВ В ПРОЕКТ
# ===========================

add_library(${PROJECT_NAME}_objs OBJECT
    # Утилиты
    src/utils/jwt.hpp
    src/utils/jwt.cpp
    src/utils/filename_generator.hpp
    src/utils/filename_generator.cpp    
    src/utils/file_manager.hpp
    src/utils/file_manager.cpp
    src/utils/data_to_text_formatter.hpp
    src/utils/data_to_text_formatter.cpp
    src/utils/cors_middleware.cpp
    src/utils/pipeline_builder.cpp

    # gRPC - клиенты
    src/grpc_clients/grpc_sync_client.hpp
    src/grpc_clients/grpc_sync_client.cpp
    src/grpc_clients/grpc_langchain_client.hpp
    src/grpc_clients/grpc_langchain_client.cpp
    src/grpc_clients/grpc_tag_recommender_client.hpp
    src/grpc_clients/grpc_tag_recommender_client.cpp

    # Аутентификация
    src/api/auth/auth_bearer.hpp
    src/api/auth/auth_bearer.cpp

    # DTO (Data Transfer Objects)
    src/dto/user.hpp
    src/dto/user.cpp
    src/dto/history.hpp
    src/dto/history.cpp
    src/dto/note.hpp
    src/dto/note.cpp
    src/dto/checklist.cpp
    src/dto/checklist.hpp
    src/dto/item.hpp
    src/dto/item.cpp
    src/dto/attachment.hpp
    src/dto/attachment.cpp
    src/dto/search_params.hpp
    src/dto/search_params.cpp

    # Модели данных
    src/models/query.hpp
    src/models/query.cpp
    src/models/note.hpp
    src/models/note.cpp
    src/models/user.hpp
    src/models/tag.cpp
    src/models/tag.hpp
    src/models/checklist.cpp
    src/models/checklist.hpp
    src/models/attachment.hpp

    # Эндпоинты
    src/api/handlers/register.hpp
    src/api/handlers/register.cpp
    src/api/handlers/login.hpp
    src/api/handlers/login.cpp
    src/api/handlers/history.hpp
    src/api/handlers/history.cpp
    src/api/handlers/note.hpp
    src/api/handlers/note.cpp
    src/api/handlers/tag.hpp
    src/api/handlers/tag.cpp
    src/api/handlers/checklist.hpp
    src/api/handlers/checklist.cpp
    src/api/handlers/checklist_item.hpp
    src/api/handlers/checklist_item.cpp
    src/api/handlers/attachment.hpp
    src/api/handlers/attachment.cpp
    src/api/handlers/notes.hpp
    src/api/handlers/notes.cpp
    src/api/handlers/suggest.hpp
    src/api/handlers/suggest.cpp
    src/api/handlers/ai_answer.hpp
    src/api/handlers/ai_answer.cpp

    # Сервис для поиска заметок
    src/search_service/search_service.cpp
    src/search_service/search_strategy.cpp
    src/search_service/search_strategy.hpp
    src/search_service/strategies/exact_search.cpp
    src/search_service/strategies/none_search.cpp
    src/search_service/strategies/semantic_search.cpp
)


# ===============================
# ДОБАВЛЕНИЕ И ЛИНКОВКА БИБЛИОТЕК 
# ===============================

# Добавляем библиотеки postgresql и grpc
target_link_libraries(${PROJECT_NAME}_objs PUBLIC userver::postgresql userver::grpc)

# JWT настройка конфига
set(CPP_JWT_BUILD_TESTS OFF)
set(CPP_JWT_BUILD_EXAMPLES OFF)

# Добавление JWT и libbcrypt поддиректорий
add_subdirectory(third_party/cpp-jwt)
add_subdirectory(third_party/bcrypt)
option(JWT_BUILD_EXAMPLES OFF)

# Линковка jwt библиотеки 
target_include_directories(${PROJECT_NAME}_objs PUBLIC cpp-jwt)
target_link_libraries(${PROJECT_NAME}_objs PUBLIC cpp-jwt)

# Линковка bcrypt библиотеки 
target_include_directories(${PROJECT_NAME}_objs PUBLIC bcrypt)
target_link_libraries(${PROJECT_NAME}_objs PUBLIC bcrypt)

# Сервис
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_objs)


# =======================
# ДОБАВЛЕНИЕ ПРОТО ФАЙЛОВ 
# =======================
userver_add_grpc_library(${PROJECT_NAME}-proto PROTOS samples/SyncService.proto
						      samples/Langchain.proto
						      samples/TagRecommender.proto)
target_link_libraries(${PROJECT_NAME}_objs PUBLIC ${PROJECT_NAME}-proto)



# =================
# ДОБАВЛЕНИЕ ТЕСТОВ
# =================

# Юнит тесты
add_executable(${PROJECT_NAME}_unittest
    src/unit-tests/jwt_test.cpp
    src/unit-tests/bcrypt_test.cpp
    src/unit-tests/filename_generator_test.cpp
    src/unit-tests/filemanager_test.cpp
)

# Линковка библиотеки юнит тетсов 
target_link_libraries(${PROJECT_NAME}_unittest PRIVATE ${PROJECT_NAME}_objs userver::utest)
add_google_tests(${PROJECT_NAME}_unittest)


# Функциональное тестирование - питон 
userver_testsuite_add_simple(
  SERVICE_TARGET ${PROJECT_NAME}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests
  PYTHON_BINARY ${TESTSUITE_PYTHON_BINARY}
  PYTEST_ARGS
  --service-secdist=${CMAKE_SOURCE_DIR}/configs/jwt_config.json
)

# ==============================
# КОНФИГУРАЦИЯ УСТАНОВКИ ПРОЕКТА
# ==============================

# Проверка и установка префикса для установки 
if(DEFINED ENV{PREFIX})
  message(STATUS "Set install prefix: $ENV{PREFIX}")
  file(TO_CMAKE_PATH "$ENV{PREFIX}" PREFIX_PATH)
  set(CMAKE_INSTALL_PREFIX "${PREFIX_PATH}")
endif()

# Сборка списка конфигурационных файлов (YAML и JSON) из директории configs
file(GLOB CONFIGS_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.yaml 
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.json)

# Установка основных исполняемых файлов проекта
install(TARGETS ${PROJECT_NAME} 
    DESTINATION ${CMAKE_INSTALL_BINDIR} 
    COMPONENT ${PROJECT_NAME})

# Копируем конфигурационные файлы в системную директорию конфигураций
install(FILES ${CONFIGS_FILES} 
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME} 
    COMPONENT ${PROJECT_NAME})
