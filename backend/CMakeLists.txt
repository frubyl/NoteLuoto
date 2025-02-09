cmake_minimum_required(VERSION 3.12)
project(noteluoto CXX)

include(GNUInstallDirs)

# JWT build conf
set(CPP_JWT_BUILD_TESTS OFF)
set(CPP_JWT_BUILD_EXAMPLES OFF)
# add JWT and libbcrypt subdirectory
add_subdirectory(third_party/cpp-jwt)
add_subdirectory(third_party/bcrypt)

option(JWT_BUILD_EXAMPLES OFF)


# Adding userver dependency
find_package(userver COMPONENTS core grpc postgresql QUIET)
if(NOT userver_FOUND)  # Fallback to subdirectory usage
    # Enable userver libraries that are needed in this project
    set(USERVER_FEATURE_POSTGRESQL ON CACHE BOOL "" FORCE)
    set(USERVER_FEATURE_GRPC ON CACHE BOOL "" FORCE)

    # Compatibility mode: some systems don't support these features
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


# Common sources
add_library(${PROJECT_NAME}_objs OBJECT
    src/utils/jwt.hpp
    src/utils/jwt.cpp
    src/utils/make_errors.hpp
    src/utils/make_errors.cpp

    src/api/auth/auth_bearer.hpp
    src/api/auth/auth_bearer.cpp

    src/api/handlers/register.hpp
    src/api/handlers/register.cpp
    src/api/handlers/login.hpp
    src/api/handlers/login.cpp
    src/api/handlers/history.hpp
    src/api/handlers/history.cpp

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


    src/models/query.hpp
    src/models/query.cpp
    src/models/note.hpp
    src/models/note.cpp
    src/models/user.hpp
    src/models/tag.cpp
    src/models/tag.hpp
    src/models/checklist.cpp
    src/models/checklist.hpp

    src/api/handlers/create_note.hpp
    src/api/handlers/create_note.cpp
    src/api/handlers/notes_id.hpp
    src/api/handlers/notes_id.cpp

    src/api/handlers/tags_create.hpp
    src/api/handlers/tags_create.cpp
    src/api/handlers/tags_all.hpp
    src/api/handlers/tags_all.cpp
    src/api/handlers/tags_note.hpp
    src/api/handlers/tags_note.cpp
    src/api/handlers/tags_note_tag.hpp
    src/api/handlers/tags_note_tag.cpp

    src/api/handlers/checklist.hpp
    src/api/handlers/checklist.cpp

    src/api/handlers/checklist_item.hpp
    src/api/handlers/checklist_item.cpp
)




target_link_libraries(${PROJECT_NAME}_objs PUBLIC userver::postgresql userver::grpc)

# Link jwt librarys 
target_include_directories(${PROJECT_NAME}_objs PUBLIC cpp-jwt)
target_link_libraries(${PROJECT_NAME}_objs PUBLIC cpp-jwt)

# Link bcrypt librarys 
target_include_directories(${PROJECT_NAME}_objs PUBLIC bcrypt)
target_link_libraries(${PROJECT_NAME}_objs PUBLIC bcrypt)

# The Service
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_objs)


# Unit Tests
add_executable(${PROJECT_NAME}_unittest
    src/unit-tests/jwt_test.cpp
    src/unit-tests/bcrypt_test.cpp

)

target_link_libraries(${PROJECT_NAME}_unittest PRIVATE ${PROJECT_NAME}_objs userver::utest)
add_google_tests(${PROJECT_NAME}_unittest)


# Functional Tests
userver_testsuite_add_simple(
  SERVICE_TARGET ${PROJECT_NAME}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests
  PYTHON_BINARY ${TESTSUITE_PYTHON_BINARY}
  PYTEST_ARGS
  --service-secdist=${CMAKE_SOURCE_DIR}/configs/jwt_config.json
)



if(DEFINED ENV{PREFIX})
  message(STATUS "Set install prefix: $ENV{PREFIX}")
  file(TO_CMAKE_PATH "$ENV{PREFIX}" PREFIX_PATH)
  set(CMAKE_INSTALL_PREFIX "${PREFIX_PATH}")
endif()


file(GLOB CONFIGS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.yaml ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.json)
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${PROJECT_NAME})
install(FILES ${CONFIGS_FILES} DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME} COMPONENT ${PROJECT_NAME})
