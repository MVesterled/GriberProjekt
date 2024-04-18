cmake_minimum_required(VERSION 3.5)

project(SemesterProgram LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/" ${CMAKE_MODULE_PATH})

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  message("${PROJECT_NAME}: You did not request a specific build type: selecting 'RelWithDebInfo'.")
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

option(WITH_ASAN "Compile with address sanitizer support" OFF)

##
## Check C++11 support / enable global pedantic and Wall
##
include(DefineCXX17CompilerFlag)
DEFINE_CXX_17_COMPILER_FLAG(CXX17_FLAG)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")

if(WITH_ASAN)
  target_compile_options(urcl PUBLIC -fsanitize=address)
  target_link_options(urcl PUBLIC -fsanitize=address)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
find_package(ur_client_library REQUIRED)

add_executable(SemesterProgram
  main.cpp
  robcomm.cpp
  robcomm.h
)
target_link_libraries(SemesterProgram Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(SemesterProgram ur_client_library::urcl)
