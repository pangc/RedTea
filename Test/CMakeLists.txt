project(Test)

set(TEST_FILES
    test.cpp
    test_common.cpp
	test_core.cpp
    test_device.cpp
    test_math.cpp
)

set(TEST_LIBS
    Common
    Device
	Core
	Math
)

add_executable(Test ${TEST_FILES})
target_link_libraries(Test gtest ${TEST_LIBS})