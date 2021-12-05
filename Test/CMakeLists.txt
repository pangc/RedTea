project(Test)

set(TEST_FILES
	test.cpp
	test_common.cpp
	test_core.cpp
	test_device.cpp
	test_math.cpp
)

if(USE_VULKAN)
elseif(USE_DX12)
list(APPEND TEST_FILES test_dx.cpp)
endif()

set(TEST_LIBS
	Core
	Engine
	Math
	Common
	Device
)

add_executable(Test ${TEST_FILES})
target_link_libraries(Test gtest ${TEST_LIBS})
add_dependencies(${TARGET} ${TEST_LIBS})