cmake_minimum_required(VERSION 3.23)

project(FinalProject VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGL OpenGLWidgets)
find_package(GTest REQUIRED)

#-----------------------------------SimFluidPhysics library------------------------------

set(PHYSICS_LIBRARY_NAME "SimFluidPhysics")
add_library(${PHYSICS_LIBRARY_NAME})
target_sources(${PHYSICS_LIBRARY_NAME}
    PRIVATE
        test.cpp
    PUBLIC
        FILE_SET HEADERS
            BASE_DIRS ${PROJECT_SOURCE_DIR}
            FILES
                test.hpp
)
target_include_directories(${PHYSICS_LIBRARY_NAME}
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)

#-----------------------------------SimFluid package---------------------------------
set(GUI_PACKAGE_NAME "SimFluid")
add_executable(${GUI_PACKAGE_NAME})
target_sources(${GUI_PACKAGE_NAME}
    PRIVATE
    main.cpp
    mainwindow.cpp
    mainwindow.hpp
    mainwindow.ui
    sceneview.hpp
    sceneview.cpp
    shaders.qrc
)

target_compile_features(${GUI_PACKAGE_NAME} PUBLIC cxx_std_17)
target_include_directories(${GUI_PACKAGE_NAME} PRIVATE)

target_link_libraries(${GUI_PACKAGE_NAME}
    PRIVATE
    Qt6::Widgets
    Qt6::OpenGL
    Qt::OpenGLWidgets
    ${PHYSICS_LIBRARY_NAME}
)



#-----------------------------------UnitTests library-----------------------------------

set(TESTS_LIB_NAME "UnitTests")
add_executable(${TESTS_LIB_NAME})
target_sources(${TESTS_LIB_NAME}
    PRIVATE
        unitTests_test.cpp
)

target_include_directories(${TESTS_LIB_NAME}
    PRIVATE
        "${GTEST_INCLUDE_DIRS}")

target_link_libraries(${TESTS_LIB_NAME}
    PRIVATE
        ${GTEST_LIBRARIES}
        ${GTEST_MAIN_LIBRARIES}
        ${PHYSICS_LIBRARY_NAME}
)

















