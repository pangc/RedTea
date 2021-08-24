set(PROJECT_NAME ISPC)
project(${PROJECT_NAME} C CXX)

#设置ISPC文件
set (ISPC_SRC_NAME
    simple
)

#设置cpp文件
set(SOURCE_FILES
    simple.cpp
)

set(HEADER_FILES
)

#默认O2优化
set (ISPC_FLAGS -O2)
#设置X86上使用SSE
set (ISPC_IA_TARGETS "sse2")
#设置ARM上使用NEON
set (ISPC_ARM_TARGETS "neon")

#设置编译工具路径
if (WIN32)
set (ISPC_EXECUTABLE "${ENGINE_ROOT_DIR}/Tools/ispc/windows/ispc.exe")
elseif (APPLE)
set (ISPC_EXECUTABLE "${ENGINE_ROOT_DIR}/Tools/ispc/mac/ispc")
else()
set (ISPC_EXECUTABLE "${ENGINE_ROOT_DIR}/Tools/ispc/linux/ispc")
endif()

#读取系统信息
if (UNIX)
    if(NOT ISPC_ARCH)
        execute_process( COMMAND bash "-c" "uname -m | sed -e s/x86_64/x86/ -e s/amd64/x86/ -e s/i686/x86/ -e s/arm64/aarch64/ -e s/arm.*/arm/ -e s/sa110/arm/" OUTPUT_VARIABLE ARCH)

        string(STRIP ${ARCH} ARCH)
        execute_process( COMMAND getconf LONG_BIT OUTPUT_VARIABLE ARCH_BIT)
        string(STRIP ${ARCH_BIT} arch_bit)
        if ("${ARCH}" STREQUAL "x86")
            if (${arch_bit} EQUAL 32)
                set(ispc_arch "x86")
            else()
                set(ispc_arch "x86-64")
            endif()
        elseif ("${ARCH}" STREQUAL "arm")
            set(ispc_arch "arm")
        elseif ("${ARCH}" STREQUAL "aarch64")
            set(ispc_arch "aarch64")
        endif()
    endif()

    set(ISPC_ARCH "${ispc_arch}" CACHE STRING "ISPC CPU ARCH")
    set(ISPC_ARCH_BIT "${arch_bit}" CACHE STRING "ISPC CPU BIT")
else()
    if(NOT ISPC_ARCH)
        set(ispc_arch "x86")
        if (CMAKE_SIZEOF_VOID_P EQUAL 8 )
            set(ispc_arch "x86-64")
        endif()
    endif()

    set(ISPC_ARCH "${ispc_arch}" CACHE STRING "ISPC CPU ARCH")
    set(ISPC_ARCH_BIT "${arch_bit}" CACHE STRING "ISPC CPU BIT")
endif()

#根据架构选择指令集
if ("${ISPC_ARCH}" MATCHES "x86")
    set(ISPC_TARGETS ${ISPC_IA_TARGETS})
elseif ("${ISPC_ARCH}" STREQUAL "arm" OR "${ISPC_ARCH}" STREQUAL "aarch64")
    set(ISPC_TARGETS ${ISPC_ARM_TARGETS})
else()
    message(FATAL_ERROR "Unknown architecture ${ISPC_ARCH}")
endif()

if (UNIX)
    list(APPEND ISPC_FLAGS --pic)
endif()

#编译每一个ISPC文件
set(ALL_ISPC_BUILD_OUTPUT)
set(ISPC_SOURCE_LIST)
foreach(source_name  ${ISPC_SRC_NAME})
	set(ISPC_BUILD_OUTPUT)
	set(ISPC_HEADER_NAME "${CMAKE_CURRENT_SOURCE_DIR}/${source_name}_ispc.h")
	set(ISPC_OBJ_NAME "${CMAKE_CURRENT_BINARY_DIR}/${source_name}.ispc${CMAKE_CXX_OUTPUT_EXTENSION}")
	
	list(APPEND ISPC_BUILD_OUTPUT ${ISPC_HEADER_NAME} ${ISPC_OBJ_NAME})

	#生成ISPC头文件和.o
	add_custom_command(OUTPUT ${ISPC_BUILD_OUTPUT}
    COMMAND ${ISPC_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${source_name}.ispc ${ISPC_FLAGS} --target=${ISPC_TARGETS} --arch=${ISPC_ARCH}
                                -h ${ISPC_HEADER_NAME} -o ${ISPC_OBJ_NAME}
    VERBATIM
    DEPENDS ${ISPC_EXECUTABLE}
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${source_name}.ispc")

    #记录生成的文件
	list(APPEND ALL_ISPC_BUILD_OUTPUT ${ISPC_BUILD_OUTPUT})
    list(APPEND ISPC_SOURCE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/${source_name}.ispc)
endforeach()

if (WIN32)
    set_source_files_properties("${ISPC_SOURCE_LIST}" PROPERTIES HEADER_FILE_ONLY TRUE)
endif()

add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${ALL_ISPC_BUILD_OUTPUT} ${ISPC_SOURCE_LIST})
target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# Set C++ standard to C++11.
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES)

# Compile options
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
if (UNIX)
    if (${ISPC_ARCH_BIT} EQUAL 32)
        target_compile_options(${PROJECT_NAME} PRIVATE -m32)
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE -m64)
    endif()
else()
    target_compile_options(${PROJECT_NAME} PRIVATE /fp:fast /Oi)
endif()