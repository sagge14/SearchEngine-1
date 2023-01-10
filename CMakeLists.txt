cmake_minimum_required(VERSION 3.24)
project(SearchEngine)

set(CMAKE_CXX_STANDARD 20)

set(MY_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(MY_SRC_DIR ${CMAKE_SOURCE_DIR}/src)


include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_executable(SearchEngine main.cpp src/ConverterJSON.cpp include/ConverterJSON.h src/InvertedIndex.cpp include/InvertedIndex.h src/SearchServer.cpp include/SearchServer.h)
enable_testing()
add_subdirectory(tests)
add_subdirectory(src)

target_link_libraries(SearchEngine
        PRIVATE nlohmann_json::nlohmann_json)

FetchContent_MakeAvailable(json)