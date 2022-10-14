file(GLOB_RECURSE SRC_SOURCES LIST_DIRECTORIES false *.h *.cpp)

add_library(Minesolver_src ${SRC_SOURCES})

target_link_libraries(Minesolver_src PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
)

target_include_directories(Minesolver_src PRIVATE
    ../include
)

target_include_directories(Minesolver_src PUBLIC
    game
    solver
    ui
    utils
)
