cmake_minimum_required(VERSION 3.24)
project(SearchEngine)

set(CMAKE_CXX_STANDARD 20)

set(MY_SRC_DIR                      ${CMAKE_SOURCE_DIR}/src)
set(MY_INCLUDE_DIR                  ${CMAKE_SOURCE_DIR}/include)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR/bin})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR})



#message(WARNING "CMAKE_SOURCE_DIR   = " ${CMAKE_SOURCE_DIR})
#message(WARNING "MY_SRC_DIR         = " ${MY_SRC_DIR})
#message(WARNING "MY_INCLUDE_DIR     = " ${MY_INCLUDE_DIR})
#message(WARNING "CMAKE_RUNTIME_OUTPUT_DIRECTORY = " ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

include(FetchContent)

FetchContent_Declare(
        json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)

include_directories(SearchEngine ${MY_INCLUDE_DIR})
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_executable(SearchEngine
        src/main.cpp
        src/ConverterJSON.cpp
        include/ConverterJSON.h
        src/InvertedIndex.cpp
        include/InvertedIndex.h
        src/SearchServer.cpp
        include/SearchServer.h)

enable_testing()
add_subdirectory(tests)

target_link_libraries(SearchEngine nlohmann_json::nlohmann_json)

FetchContent_MakeAvailable(json googletest)
