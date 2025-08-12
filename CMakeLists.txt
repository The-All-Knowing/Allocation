cmake_minimum_required(VERSION 3.12)
project(allocation LANGUAGES CXX)

find_package(Poco REQUIRED)
find_package(GTest REQUIRED)

# Common sources
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.[hc]pp)
file(GLOB_RECURSE UNIT_TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.[hc]pp)
list(REMOVE_ITEM SOURCES ${UNIT_TEST_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_library(${PROJECT_NAME}_objs OBJECT ${SOURCES})
include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${PROJECT_NAME}_unittest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${PROJECT_NAME}_unittest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tests)
target_link_libraries(${PROJECT_NAME}_objs PUBLIC Poco::Poco)

# The Service
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_objs)

# Unit Tests
enable_testing()
add_executable(${PROJECT_NAME}_unittest ${UNIT_TEST_SOURCES})
target_link_libraries(${PROJECT_NAME}_unittest PRIVATE ${PROJECT_NAME}_objs GTest::gtest_main GTest::gmock)
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_unittest)

# Install
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

install(TARGETS ${PROJECT_NAME} DESTINATION "/mnt/c/Projects/Allocation/bin" COMPONENT ${PROJECT_NAME})
install(FILES ${CONFIGS_FILES} DESTINATION "/mnt/c/Projects/Allocation/bin" COMPONENT ${PROJECT_NAME})