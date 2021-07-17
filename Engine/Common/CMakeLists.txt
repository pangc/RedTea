set(TARGET Common)


set(HEADER_FILES
    common.h
    compiler_option.h
    object.h
    event.h
    logger/logger.h
    logger/ostream.h
    utils/struct_of_arrays.h
    utils/memory.h
    utils/memory.cpp
)

set(SOURCE_FILES
    object.cpp
    logger/logger.cpp
    logger/ostream.cpp
)

add_library(${TARGET} STATIC ${HEADER_FILES}  ${SOURCE_FILES})
target_include_directories(${TARGET} PUBLIC .)
