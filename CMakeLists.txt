cmake_minimum_required(VERSION 3.16)
project(Minesolver VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Widgets)

qt_standard_project_setup()

qt_add_executable(Minesolver WIN32 MACOSX_BUNDLE
    src/game/MineStatus.h
    src/game/Minefield.cpp src/game/Minefield.h
    src/game/TraversableGrid.cpp src/game/TraversableGrid.h
    src/solver/ChoiceColumn.cpp src/solver/ChoiceColumn.h
    src/solver/ChoiceNode.cpp src/solver/ChoiceNode.h
    src/solver/PathChooser.cpp src/solver/PathChooser.h
    src/solver/PotentialMinefield.cpp src/solver/PotentialMinefield.h
    src/solver/Solver.cpp src/solver/Solver.h
    src/ui/CellDisplay.cpp src/ui/CellDisplay.h
    src/ui/MainWindow.cpp src/ui/MainWindow.h
    src/ui/MinefieldDisplay.cpp src/ui/MinefieldDisplay.h
    src/utils/RandomNumbers.cpp src/utils/RandomNumbers.h
)

target_include_directories(Minesolver PRIVATE
    include include
    src/game
    src/solver
    src/ui
    src/utils
)

target_link_libraries(Minesolver PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
)

#if(CONFIG(test))
#    target_sources(Minesolver PUBLIC
#        test/TestMain.cpp
#        test/TestingFramework.h
#    )

#    target_include_directories(Minesolver PRIVATE
#        test
#    )

#    target_link_libraries(Minesolver PRIVATE
#        gtest
#    )
#endif()

target_sources(Minesolver PUBLIC
    src/main/SolverMain.cpp
)

target_include_directories(Minesolver PRIVATE
    src/main
)

if(debug)
    target_link_libraries(Minesolver PRIVATE
        # Remove: L${CMAKE_CURRENT_SOURCE_DIR}/lib/debug
    )
endif()

if(release)
    target_link_libraries(Minesolver PRIVATE
        # Remove: L${CMAKE_CURRENT_SOURCE_DIR}/lib/release
    )
endif()

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
