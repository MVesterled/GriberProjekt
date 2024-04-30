cmake_minimum_required(VERSION 3.14)

project(Gripper LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
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

    # Specify the path to WiringPi include directory
    set(WIRINGPI_INCLUDE_DIRS "/home/pi/WiringPi")

    # Set the include directory for WiringPi
    include_directories(${WIRINGPI_INCLUDE_DIRS})

    # Set the include directory
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(Gripper
  main.cpp  Motor.cpp   Motor.h Database.cpp Database.h Encoder.h   Encoder.cpp
)
target_link_libraries(Gripper Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(Gripper ${MYSQLCONNECTORCPP_LIBRARIES} ${WIRINGPI_LIBRARY})
target_link_libraries(ur_client_library::urcl)

install(TARGETS Gripper
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

#hest






















