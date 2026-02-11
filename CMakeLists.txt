cmake_minimum_required(VERSION 3.16)

project(BoschSimulator VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. ADD QuickControls2 HERE
find_package(Qt6 REQUIRED COMPONENTS Quick Network QuickControls2)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appBoschSimulator
    main.cpp
    boschserver.cpp
    boschserver.h
)

qt_add_qml_module(appBoschSimulator
    URI BoschSimulator
    VERSION 1.0
    RESOURCE_PREFIX "/"
    QML_FILES
        Main.qml
        MicComponent.qml
)

set_target_properties(appBoschSimulator PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# 2. LINK QuickControls2 HERE
target_link_libraries(appBoschSimulator
    PRIVATE Qt6::Quick
    PRIVATE Qt6::Network
    PRIVATE Qt6::QuickControls2
)

include(GNUInstallDirs)
install(TARGETS appBoschSimulator
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
