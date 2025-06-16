cmake_minimum_required(VERSION 3.30)
project(Allocation LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/Release)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Poco REQUIRED)
find_package(GTest REQUIRED)

set(CONFIG_FILE ${CMAKE_SOURCE_DIR}/src/Infrastructure/Config/Allocation.ini)

set(allocation_sources
  src/main.cpp
  src/Precompile.cpp
  src/Utilities/Common.cpp
  src/Domain/Product/Batch.cpp
  src/Domain/Product/Product.cpp
  src/Adapters/Database/DbTables.cpp
  src/Adapters/Repository/SqlRepository.cpp
  src/Adapters/Repository/TrackingRepository.cpp
  src/Adapters/Repository/FakeRepository.cpp
  src/Adapters/Database/Session/SessionPool.cpp
  src/Adapters/Database/Mappers/OrderLineMapper.cpp
  src/Adapters/Database/Mappers/BatchMapper.cpp
  src/Adapters/Database/Mappers/ProductMapper.cpp
  src/Adapters/Redis/RedisEventPublisher.cpp
  src/Adapters/Email/Email.cpp
  src/Services/UoW/AbstractUnitOfWork.cpp
  src/Services/UoW/SqlUnitOfWork.cpp
  src/Services/MessageBus/MessageBus.cpp
  src/Services/MessageBus/Handlers.cpp
  src/Services/Loggers/ILogger.cpp
  src/Services/Views.cpp
  src/Infrastructure/Server/Server.cpp
  src/Infrastructure/Server/HandlerFactory.cpp
  src/Infrastructure/Server/Handlers/AllocateHandler.cpp
  src/Infrastructure/Server/Handlers/NotFoundHandler.cpp
  src/Infrastructure/Server/Handlers/AddBatchHandler.cpp
  src/Infrastructure/Server/Handlers/AllocationsViewHandler.cpp
  src/Infrastructure/Redis/RedisListenerModule.cpp
  src/Infrastructure/Redis/RedisConfig.cpp
)

set(project_includes
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Utilities
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Domain
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Services
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Adapters/Database
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Adapters/Repository
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Infrastructure/Server
)

add_executable(Allocation ${allocation_sources})
target_include_directories(Allocation PRIVATE ${project_includes})
target_link_libraries(Allocation PRIVATE Poco::Poco)

add_custom_command(TARGET Allocation POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CONFIG_FILE}
        $<TARGET_FILE_DIR:Allocation>
)

### Тесты
enable_testing()

set(test_sources
  src/Precompile.cpp
  src/Utilities/Common.cpp
  src/Domain/Product/Batch.cpp
  src/Domain/Product/Product.cpp
  src/Adapters/Database/DbTables.cpp
  src/Adapters/Repository/SqlRepository.cpp
  src/Adapters/Repository/TrackingRepository.cpp
  src/Adapters/Repository/FakeRepository.cpp
  src/Adapters/Database/Session/SessionPool.cpp
  src/Adapters/Database/Mappers/OrderLineMapper.cpp
  src/Adapters/Database/Mappers/BatchMapper.cpp
  src/Adapters/Database/Mappers/ProductMapper.cpp
  src/Adapters/Redis/RedisEventPublisher.cpp
  src/Adapters/Email/Email.cpp
  src/Services/UoW/AbstractUnitOfWork.cpp
  src/Services/UoW/SqlUnitOfWork.cpp
  src/Services/MessageBus/MessageBus.cpp
  src/Services/MessageBus/Handlers.cpp
  src/Services/Loggers/ILogger.cpp
  src/Services/Views.cpp
  src/Infrastructure/Redis/RedisConfig.cpp
  Tests/Utilities/CommonFunctions.cpp
  Tests/Utilities/SqlFixture.cpp
  Tests/Utilities/ApiClient.cpp
  Tests/Utilities/RedisClient.cpp
  Tests/Unit/TestProduct.cpp
  Tests/Unit/TestBatches.cpp
  Tests/Unit/TestHandlers.cpp
  Tests/Integration/TestUnitOfWork.cpp
  Tests/Integration/TestRepository.cpp
  Tests/Integration/TestViews.cpp
  Tests/E2E/TestAPI.cpp
  #Tests/E2E/TestExternalEvents.cpp
  Tests/mainTests.cpp
)

add_executable(Tests ${test_sources})
target_include_directories(Tests PRIVATE ${project_includes} ${CMAKE_CURRENT_SOURCE_DIR}/Tests/Utilities)
target_link_libraries(Tests PRIVATE GTest::gtest_main GTest::gmock Poco::Poco)

# Также для тестов
add_custom_command(TARGET Tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CONFIG_FILE}
        $<TARGET_FILE_DIR:Tests>
)

include(GoogleTest)
gtest_discover_tests(Tests)
