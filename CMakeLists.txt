# This is the root CMakeLists, it does the general Qt set up for everything
# and includes the other CMakeLists through add_subdirectory

cmake_minimum_required(VERSION 3.16)
project(Minesolver VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_LIST_DIR}/dist)

find_package(Qt6 REQUIRED COMPONENTS Core Concurrent Gui Widgets Quick)

qt_standard_project_setup()

add_subdirectory(src/core)
add_subdirectory(src/gui)

add_subdirectory(test EXCLUDE_FROM_ALL)

install(TARGETS Minesolver
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_qml_app_script(
    TARGET Minesolver
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
