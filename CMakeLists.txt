cmake_minimum_required(VERSION 3.30)
project(Allocation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


### Тесты
enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set (source
  src/Precompile.cpp
  src/Utilities/Common.cpp
  src/Domain/Source/Batch.cpp
  src/Domain/Source/OrderlLine.cpp
  Tests/Utilities/CommonFunctions.cpp
  Tests/TestAllocate.cpp
  Tests/TestBatches.cpp
)

add_executable(Tests ${source})

target_include_directories(Tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Utilities
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Domain/Include
  ${CMAKE_CURRENT_SOURCE_DIR}/Tests/Utilities)

target_link_libraries(Tests GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(Tests)