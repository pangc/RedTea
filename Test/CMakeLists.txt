project(Test)

set(TEST_FILES
    test.cpp
    test_soa.cpp
)

set(TEST_LIBS
    Common
    Device
)

add_executable(Test ${TEST_FILES})
target_link_libraries(Test gtest ${TEST_LIBS})