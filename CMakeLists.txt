cmake_minimum_required(VERSION 3.12)
PROJECT(shadabase)

add_subdirectory(./example/http_client)
add_subdirectory(./example/post_coroutine)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/shadabase DESTINATION include)