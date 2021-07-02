set(TARGET Engine)
set(APPLICATION_FILES
    Application/RedteaApp.h
    Application/RedteaApp.cpp
)
SOURCE_GROUP(Application FILES ${APPLICATION_FILES})

set(RUNTIME_LIBS
    Common
    SDL2-static
    SDL2main
)

set(INCLUDE_PATH
    ../Common/
)

add_library(${TARGET} STATIC ${APPLICATION_FILES})
target_link_libraries(${TARGET} PRIVATE ${RUNTIME_LIBS})
target_include_directories(${TARGET} PRIVATE ${INCLUDE_PATH})
add_dependencies(${TARGET} ${RUNTIME_LIBS})