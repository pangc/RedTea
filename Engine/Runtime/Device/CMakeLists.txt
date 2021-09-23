set(TARGET Device)

set(HEADER_FILES
	native_window.h
    window.h
)

set(SOURCE_FILES
    window.cpp
)

if(MSVC)
list(APPEND SOURCE_FILES native_window_win.cpp)
endif()

set(RHI_FILES
    RHI/command_buffer.h
	RHI/command_buffer.cpp
	RHI/rhi.h
	RHI/resource.h
	RHI/device.h
)

if (USE_OPENGL)
set(GLES_FILES 
    Backend/opengl/gl_device.h
    Backend/opengl/gl_device.cpp
	Backend/opengl/gl_context.h
    Backend/opengl/gl_context.cpp
	Backend/opengl/gl_resource.h
    Backend/opengl/gl_resource.cpp
	Backend/opengl/gl_platform.h
	Backend/opengl/egl_platform.h
    Backend/opengl/egl_platform.cpp
	
)
list(APPEND RHI_FILES ${GLES_FILES})
endif()
SOURCE_GROUP(RHI FILES ${RHI_FILES})

set(DEVICE_LIB
    SDL2-static
    SDL2main
    Common
)

set(COMPILED_LIB)
set(INCLUDE_PATH)
set(DYNAMIC_LIB)

if (USE_OPENGL)
find_package(OpenGL REQUIRED)
list(APPEND COMPILED_LIB ${OPENGL_LIBRARIES})
if(NOT ${OpenGL_EGL_FOUND})
	if(MSVC)
		set(EGL_LIBRARY_PATH ${ROOT_PATH}/ThirdParty/egl)
		list(APPEND COMPILED_LIB ${EGL_LIBRARY_PATH}/libEGL.lib ${EGL_LIBRARY_PATH}/libGLESv2.lib)
		list(APPEND INCLUDE_PATH ${EGL_LIBRARY_PATH}/include)
	endif()
endif()
endif()

add_library(${TARGET} STATIC ${DYNAMIC_LIB} ${HEADER_FILES}  ${SOURCE_FILES} ${RHI_FILES})
target_link_libraries(${TARGET} PRIVATE ${DEVICE_LIB} ${COMPILED_LIB})
target_include_directories(${TARGET}  PUBLIC ${INCLUDE_PATH})
add_dependencies(${TARGET} ${DEVICE_LIB})

