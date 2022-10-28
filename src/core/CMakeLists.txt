# This CMakeLists sets up a target to build the core non-gui functionality as a library
# for inclusion in the gui and testing executables

file(GLOB_RECURSE SRC_SOURCES LIST_DIRECTORIES false *.h *.cpp)

add_library(Minesolver_src ${SRC_SOURCES})

target_link_libraries(Minesolver_src PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt6::Concurrent
)

target_include_directories(Minesolver_src PUBLIC
    ../../../boost_1_80_0
    game
    solver
    ui
    utils
)
