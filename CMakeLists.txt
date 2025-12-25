cmake_minimum_required(VERSION 3.20)

project(cppServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find threads package
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Add the executable
add_executable(server_main
    cmd/main.cpp
    cmd/tcpListner.cpp
    server/server.cpp
)

# Include directories
target_include_directories(server_main PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/cmd
    ${CMAKE_CURRENT_SOURCE_DIR}/server
)

# Link libraries
target_link_libraries(server_main PRIVATE Threads::Threads)
