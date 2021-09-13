set(TARGET Engine)

add_subdirectory(Device)

set(APPLICATION_FILES
    Application/redtea_app.h
    Application/redtea_app.cpp
)
SOURCE_GROUP(Application FILES ${APPLICATION_FILES})

set(RUNTIME_LIBS
    Common
    Device
)

set(INCLUDE_PATH
    ../Common/
)


add_library(${TARGET} STATIC ${APPLICATION_FILES})
target_link_libraries(${TARGET} PRIVATE ${RUNTIME_LIBS})
target_include_directories(${TARGET} PRIVATE ${INCLUDE_PATH})
add_dependencies(${TARGET} ${RUNTIME_LIBS})