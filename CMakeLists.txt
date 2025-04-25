cmake_minimum_required(VERSION 3.10)
project(http_server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#JSON LIBRARY
include(FetchContent)
FetchContent_Declare(
	json
	GIT_REPOSITORY https://github.com/nlohmann/json.git 
	GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
	Catch2
	GIT_REPOSITORY  https://github.com/catchorg/Catch2.git
	GIT_TAG v2.13.7
)

FetchContent_MakeAvailable(Catch2)

find_package(CURL QUIET)
if(NOT CURL_FOUND)
		find_package(CURL REQUIRED NAMES libcurl)

		if(NOT CURL_FOUND)
				message(FATAL_ERROR "curl not found!")
		endif()
endif()

find_package(PkgConfig REQUIRED)
pkg_search_module(CURL REQUIRED libcurl)

# Source files
file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.hpp")

add_executable(http_server ${SOURCES} ${HEADERS})

target_link_directories(http_server PRIVATE nlohmann_json::nlohmann_json)

file(COPY public/ DESTINATION ${CMAKE_BINARY_DIR}/public)

enable_testing()

add_executable(test_server  test/test_server.cpp ${SOURCES} ${HEADERS})

target_link_libraries(test_server PRIVATE Catch2::Catch2 CURL::libcurl nlohmann_json::nlohmann_json)

add_test(NAME http_server COMMAND test_server)

