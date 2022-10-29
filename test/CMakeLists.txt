# This CMakeLists is for building the testing executable so the core functions can be tested
# I hesitate to call it unit testing as the only tests currently verify the Solver as a whole

cmake_minimum_required(VERSION 3.14)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)

add_executable(Minesolver_test
    ${TEST_SOURCES})

target_link_libraries(Minesolver_test PRIVATE
    Minesolver_src
    GTest::gtest_main
)
