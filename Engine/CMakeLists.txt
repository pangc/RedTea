# 指定生成目标
project(Client)

set(HEADER_FILES
)

add_subdirectory(Common)
add_subdirectory(Runtime)

set(SOURCE_FILES
    _main.cpp
)

set(INCLUDE_PATH
    Runtime/
    Common/
)

add_executable(Client ${HEADER_FILES} ${SOURCE_FILES})
target_include_directories(Client PRIVATE ${INCLUDE_PATH})
target_link_libraries(Client PRIVATE Engine)