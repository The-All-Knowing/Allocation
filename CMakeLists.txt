cmake_minimum_required(VERSION 3.30)
project(Allocation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Poco REQUIRED)
find_package(GTest REQUIRED)

### Тесты
enable_testing()

set (source
  src/Precompile.cpp
  src/Utilities/Common.cpp
  src/Domain/Source/Batch.cpp
  src/Domain/Source/OrderlLine.cpp
  src/Adapters/Database/Source/Common.cpp
  src/Adapters/Database/Source/Mappers/OrderLineMapper.cpp
  Tests/Utilities/CommonFunctions.cpp
  Tests/TestAllocate.cpp
  Tests/TestBatches.cpp
  Tests/TestDataMappers.cpp
)

add_executable(Tests ${source})

target_include_directories(Tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Utilities
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Domain/Include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Adapters/Database/Include
  ${CMAKE_CURRENT_SOURCE_DIR}/Tests/Utilities)

target_link_libraries(Tests PRIVATE GTest::gtest_main GTest::gmock Poco::Poco)

include(GoogleTest)
gtest_discover_tests(Tests)