cmake_minimum_required(VERSION 3.12)
project(allocation LANGUAGES CXX)

SET(CMAKE_CXX_STANDARD 23)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Poco REQUIRED)
find_package(GTest REQUIRED)

# Общие исходники
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.[hc]pp)
file(GLOB_RECURSE UNIT_TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*_test.[hc]pp)
list(REMOVE_ITEM SOURCES ${UNIT_TEST_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_library(${PROJECT_NAME}_objs OBJECT ${SOURCES})
include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${PROJECT_NAME}_unittest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${PROJECT_NAME}_unittest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tests)
target_link_libraries(${PROJECT_NAME}_objs PUBLIC Poco::Poco)

# Сервис
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_objs)

# Unit-тесты
enable_testing()
add_executable(${PROJECT_NAME}_test ${UNIT_TEST_SOURCES})
target_link_libraries(${PROJECT_NAME}_test PRIVATE ${PROJECT_NAME}_objs GTest::gtest_main GTest::gmock)
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_test)

# Установка
include(GNUInstallDirs)

if(DEFINED ENV{PREFIX})
    message(STATUS "Set install prefix: $ENV{PREFIX}")
    file(TO_CMAKE_PATH "$ENV{PREFIX}" PREFIX_PATH)
    set(CMAKE_INSTALL_PREFIX ${PREFIX_PATH})
endif()

file(GLOB CONFIGS_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.yaml
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.json
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.ini)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${PROJECT_NAME})
install(FILES ${CONFIGS_FILES} DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME} COMPONENT ${PROJECT_NAME})

# Добавляем цели для запуска сервиса
add_custom_target(start-debug
    COMMAND ${PROJECT_NAME} #--config configs/debug.yaml дополнить когда научу считывать конфиг файл по пути
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Запуск сервиса в режиме отладки"
)

add_custom_target(start-release
    COMMAND ${PROJECT_NAME} #--config configs/release.yaml
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Запуск сервиса в режиме релиза"
)

# Добавляем зависимости
add_dependencies(start-debug ${PROJECT_NAME})
add_dependencies(start-release ${PROJECT_NAME})
