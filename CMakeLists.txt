cmake_minimum_required(VERSION 3.22)

project(ConnectTheDots VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

set(TARGET_MAIN "ConnectTheDots")

add_executable("${TARGET_MAIN}"
    "src/main.cpp"
)

list(APPEND HEADERS_MAIN "include/placeholder.h")

target_include_directories("${TARGET_MAIN}" PRIVATE 
    $HEADERS_MAIN
)