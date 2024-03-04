cmake_minimum_required(VERSION 3.5)

project(Opgave-Pi-5-1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Opgave-Pi-5-1 main.cpp
    Pi.h
    Pi.cpp)

include(GNUInstallDirs)
install(TARGETS Opgave-Pi-5-1
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
