cmake_minimum_required(VERSION 3.19)
project(dynoStream)

set(CMAKE_CXX_STANDARD 20)

include_directories(inc)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Threads REQUIRED)
find_package(websocketpp REQUIRED)

# Force WebSocket++ to use 100% C++11 mode, so that it doesn't try to look for Boost
# (Note that under MinGW, Boost.Thread is still required, due to a bug in MinGW that prevents the C++11 version from being used)
add_definitions(-D_WEBSOCKETPP_CPP11_STRICT_)

add_executable(dynoStream
        src/main.cpp
        src/loadCells/VirtualLoadCell.cpp inc/loadCells/VirtualLoadCell.h
        src/Decimator.cpp inc/Decimator.h
        src/DataProcessor.cpp inc/DataProcessor.h
        src/server/WSServer.cpp inc/server/WSServer.h
        inc/loadCells/LoadCellDriver.h src/loadCells/SerialLoadCell.cpp inc/loadCells/SerialLoadCell.h)

target_link_libraries(dynoStream Threads::Threads)
