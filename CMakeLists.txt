cmake_minimum_required(VERSION 3.20)

project(cppServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(utils_main
  Utils/main.cpp
  Utils/tcpListner.cpp
)

target_include_directories(utils_main PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/Utils
)

# Needed for std::thread on some toolchains
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(utils_main PRIVATE Threads::Threads)

include(CTest)

if(BUILD_TESTING)
  add_executable(request_test
    internal/request/request_test.cpp
    internal/request/request.cpp
  )

  target_include_directories(request_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/internal/request
  )

  add_test(NAME request_test COMMAND request_test)
endif()
