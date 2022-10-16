cmake_minimum_required(VERSION 3.16)
project(Minesolver VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Widgets Quick)

qt_standard_project_setup()

file(GLOB MAIN_SOURCES LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_LIST_DIR} main/**.h main/**.cpp)

qt_add_executable(Minesolver WIN32 MACOSX_BUNDLE
    main/SolverMain.cpp
    main/ui_backend/AppState.h
    main/ui_backend/AppState.cpp
    main/ui_backend/MinefieldTableModel.h
    main/ui_backend/MinefieldTableModel.cpp
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
)

target_sources(Minesolver PUBLIC
    main/SolverMain.cpp
)

target_include_directories(Minesolver PRIVATE
    main
    main/ui_backend
)

add_subdirectory(src)
add_subdirectory(test)

install(TARGETS Minesolver
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET Minesolver
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
