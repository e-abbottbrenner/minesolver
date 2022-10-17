# This CMakeLists sets up the target for the GUI executable that lets the user play the game

qt_add_executable(Minesolver WIN32 MACOSX_BUNDLE
    SolverMain.cpp
    ui_backend/AppState.h
    ui_backend/AppState.cpp
    ui_backend/MinefieldTableModel.h
    ui_backend/MinefieldTableModel.cpp
)

file(GLOB QML_SOURCES LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_LIST_DIR} qml/**.qml)

qt_add_qml_module(Minesolver
    URI Minesolver
    VERSION 1.0
    QML_FILES
    ${QML_SOURCES}
)

target_link_libraries(Minesolver PRIVATE
    Minesolver_src
    Qt::Quick
)

target_sources(Minesolver PUBLIC
    SolverMain.cpp
)

target_include_directories(Minesolver PRIVATE
    ui_backend
)
