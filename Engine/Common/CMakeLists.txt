set(TARGET Common)

set(HEADER_FILES
    common.h
    object.h
    logger/logger.h
    logger/ostream.h
)

set(SOURCE_FILES
    object.cpp
    logger/logger.cpp
    logger/ostream.cpp
)


add_library(${TARGET} STATIC ${HEADER_FILES}  ${SOURCE_FILES})
target_link_libraries(${TARGET} PUBLIC ${RUNTIME_LIBS})