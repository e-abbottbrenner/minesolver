# This is the root CMakeLists, it does the general Qt set up for everything
# and includes the other CMakeLists through add_subdirectory

cmake_minimum_required(VERSION 3.16)
project(Minesolver VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Widgets Quick)

qt_standard_project_setup()

add_subdirectory(src/core)
add_subdirectory(src/gui)
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
