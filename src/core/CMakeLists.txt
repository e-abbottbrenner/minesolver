# This CMakeLists sets up a target to build the core non-gui functionality as a library
# for inclusion in the gui and testing executables

file(GLOB_RECURSE SRC_SOURCES LIST_DIRECTORIES false *.h *.cpp)

include(FetchContent)
FetchContent_Declare(
    Boost
    URL https://boostorg.jfrog.io/artifactory/main/release/1.80.0/source/boost_1_80_0.zip
    URL_HASH SHA256=e34756f63abe8ac34b35352743f17d061fcc825969a2dd8458264edb38781782
)

FetchContent_MakeAvailable(Boost)

add_library(Minesolver_src ${SRC_SOURCES})

target_link_libraries(Minesolver_src PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt6::Concurrent
)

target_include_directories(Minesolver_src PUBLIC
    ${CMAKE_BINARY_DIR}/_deps/boost-src
    game
    solver
    ui
    utils
)
