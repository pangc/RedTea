set(TARGET Core)

set(HEADER_FILES
    entity.h
    entity_manager.h
	component_manager.h
	component.h
    world.h
)

set(SOURCE_FILES
    entity.cpp
    entity_manager.cpp
    world.cpp
)
set(INCLUDE_PATH
    ../Common/
)
add_library(${TARGET} STATIC ${HEADER_FILES}  ${SOURCE_FILES})
target_link_libraries(Common)
target_include_directories(${TARGET} PRIVATE ${INCLUDE_PATH})