cmake_minimum_required(VERSION 3.19)
set(PROJECT_NAME RedTea)
set(ENGINE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
project(${PROJECT_NAME})

set (CMAKE_CXX_STANDARD 17)

set (ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

option(USE_OPENGL "Enable/Disable OpenGl on WIN32" OFF)
option(USE_DX12 "Enable/Disable DX12 on WIN32" ON)
add_subdirectory(Engine)
add_subdirectory(ThirdParty)
add_subdirectory(Test)
