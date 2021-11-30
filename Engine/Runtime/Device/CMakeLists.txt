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
	RHI/misc.h
	RHI/rhi_base.cpp
	RHI/resource.h
	RHI/containers.h
	RHI/containers.cpp
	RHI/rhi_utils.h
	RHI/rhi_utils.cpp
	RHI/state-tracking.h
	RHI/state-tracking.cpp
)

list(APPEND RHI_FILES)
if(USE_VULKAN)
elseif(USE_DX12)
set(DX_FILES
)
list(APPEND RHI_FILES ${DX_FILES})
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

if(USE_DX12)
if(MSVC)
	list(APPEND COMPILED_LIB d3d12 dxgi d3dcompiler)
endif()
endif()

add_library(${TARGET} STATIC ${DYNAMIC_LIB} ${HEADER_FILES}  ${SOURCE_FILES} ${RHI_FILES})
target_link_libraries(${TARGET} PRIVATE ${DEVICE_LIB} ${COMPILED_LIB})
target_include_directories(${TARGET}  PUBLIC ${INCLUDE_PATH})
add_dependencies(${TARGET} ${DEVICE_LIB})

