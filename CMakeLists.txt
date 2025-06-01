cmake_minimum_required(VERSION 3.30)
project(Allocation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Poco REQUIRED)
find_package(GTest REQUIRED)

set(allocation_sources
  src/main.cpp
  src/Precompile.cpp
  src/Utilities/Common.cpp
  src/Domain/Source/Batch.cpp
  src/Domain/Source/OrderlLine.cpp
  src/Adapters/Database/Source/DbTables.cpp
  src/Adapters/Repository/Source/SqlRepository.cpp
  src/Adapters/Repository/Source/FakeRepository.cpp
  src/Adapters/Database/Source/Session/SessionPool.cpp
  src/Adapters/Database/Source/Mappers/OrderLineMapper.cpp
  src/Adapters/Database/Source/Mappers/BatchMapper.cpp
  src/Services/Source/Services.cpp
  src/Infrastructure/Server/Source/Server.cpp
  src/Infrastructure/Server/Source/HandlerFactory.cpp
  src/Infrastructure/Server/Source/Handlers/AllocateHandler.cpp
  src/Infrastructure/Server/Source/Handlers/NotFoundHandler.cpp
)

set(project_includes
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Utilities
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Domain/Include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Services/Include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Adapters/Database/Include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Adapters/Repository/Include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Infrastructure/Server/Include
)

add_executable(Allocation ${allocation_sources})
target_include_directories(Allocation PRIVATE ${project_includes})
target_link_libraries(Allocation PRIVATE Poco::Poco)

### Тесты
enable_testing()

set(test_sources
  src/Precompile.cpp
  src/Utilities/Common.cpp
  src/Domain/Source/Batch.cpp
  src/Domain/Source/OrderlLine.cpp
  src/Adapters/Database/Source/DbTables.cpp
  src/Adapters/Repository/Source/SqlRepository.cpp
  src/Adapters/Repository/Source/FakeRepository.cpp
  src/Adapters/Database/Source/Session/SessionPool.cpp
  src/Adapters/Database/Source/Mappers/OrderLineMapper.cpp
  src/Adapters/Database/Source/Mappers/BatchMapper.cpp
  src/Services/Source/Services.cpp
  Tests/Utilities/CommonFunctions.cpp
  Tests/Unit/TestAllocate.cpp
  Tests/Unit/TestBatches.cpp
  Tests/Unit/TestServices.cpp
  Tests/Integration/TestDataMappers.cpp
  Tests/Integration/TestRepository.cpp
  Tests/E2E/TestAPI.cpp
)

add_executable(Tests ${test_sources})
target_include_directories(Tests PRIVATE ${project_includes} ${CMAKE_CURRENT_SOURCE_DIR}/Tests/Utilities)
target_link_libraries(Tests PRIVATE GTest::gtest_main GTest::gmock Poco::Poco)

include(GoogleTest)
gtest_discover_tests(Tests)
