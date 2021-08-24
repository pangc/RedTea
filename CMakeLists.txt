cmake_minimum_required(VERSION 3.19)
set(PROJECT_NAME RedTea)
set(ENGINE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
project(${PROJECT_NAME})

set (CMAKE_CXX_STANDARD 17)

set (ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Engine)
add_subdirectory(ThirdParty)
add_subdirectory(Test)
