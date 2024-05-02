project(GripperRobot LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/" ${CMAKE_MODULE_PATH})

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  message("${PROJECT_NAME}: You did not request a specific build type: selecting 'RelWithDebInfo'.")
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(MYSQLCONNECTORCPP_ROOT_DIR
        "${MYSQLCONNECTORCPP_ROOT_DIR}"
        CACHE
        PATH
        "Where to start looking for this component.")



if(WIN32)
    find_path(MYSQLCONNECTORCPP_INCLUDE_DIR
            NAMES
            mysql_connection.h
            PATHS
            "C:\\Program Files"
            HINTS
            ${MYSQLCONNECTORCPP_ROOT_DIR}
            PATH_SUFFIXES
            include)

    find_library(MYSQLCONNECTORCPP_LIBRARY
            NAMES
            mysqlcppconn
            mysqlcppconn-static
            HINTS
            ${MYSQLCONNECTORCPP_ROOT_DIR}
            PATH_SUFFIXES
            lib)

else()
    find_path(MYSQLCONNECTORCPP_INCLUDE_DIR
            mysql_connection.h
            HINTS
            ${MYSQLCONNECTORCPP_ROOT_DIR}
            PATH_SUFFIXES
            include)

    find_library(MYSQLCONNECTORCPP_LIBRARY
            NAMES
            mysqlcppconn
            mysqlcppconn-static
            HINTS
            ${MYSQLCONNECTORCPP_ROOT_DIR}
            PATH_SUFFIXES
            lib64
            lib)
endif()

mark_as_advanced(MYSQLCONNECTORCPP_INCLUDE_DIR MYSQLCONNECTORCPP_LIBRARY)

include(FindPackageHandleStandardArgs)

option(WITH_ASAN "Compile with address sanitizer support" OFF)

##
## Check C++11 support / enable global pedantic and Wall
##
#include(DefineCXX17CompilerFlag)
#DEFINE_CXX_17_COMPILER_FLAG(CXX17_FLAG)
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
find_package_handle_standard_args(MysqlConnectorCpp
        DEFAULT_MSG
        MYSQLCONNECTORCPP_INCLUDE_DIR
        MYSQLCONNECTORCPP_LIBRARY)

if(MYSQLCONNECTORCPP_FOUND)
    set(MYSQLCONNECTORCPP_INCLUDE_DIRS "${MYSQLCONNECTORCPP_INCLUDE_DIR}") # Add any dependencies here
    set(MYSQLCONNECTORCPP_LIBRARIES "${MYSQLCONNECTORCPP_LIBRARY}") # Add any dependencies here
    mark_as_advanced(MYSQLCONNECTORCPP_ROOT_DIR)
endif()

# Find WiringPi
find_library(WIRINGPI_LIBRARY wiringPi)
find_library(WIRINGPISPI_LIBRARY wiringPiSPI)

# Specify the path to WiringPi include directory
set(WIRINGPI_INCLUDE_DIRS "/home/pi/WiringPi")

# Set the include directory for WiringPi
include_directories(${WIRINGPI_INCLUDE_DIRS})

add_executable(GripperRobot
  main.cpp motor.h motor.cpp database.h database.cpp Encoder.h   Encoder.cpp

)
target_link_libraries(GripperRobot Qt${QT_VERSION_MAJOR}::Core ${WIRINGPI_LIBRARY} ${MYSQLCONNECTORCPP_LIBRARIES})
target_link_libraries(GripperRobot ur_client_library::urcl)
