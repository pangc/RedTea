set(TARGET Device)

set(HEADER_FILES
    window.h
)

set(SOURCE_FILES
    window.cpp
)

set(DEVICE_LIB
    SDL2-static
    SDL2main
    Common
)

add_library(${TARGET} STATIC ${HEADER_FILES}  ${SOURCE_FILES})
target_link_libraries(${TARGET} PRIVATE ${DEVICE_LIB})
add_dependencies(${TARGET} ${DEVICE_LIB})