set(TARGET Math)
project(${TARGET})

set(HEADER_FILES
	vector.h
	vector_helper.h
	matrix.h
	matrix_helper.h
	quaternion.h
	quaternion_helper.h
)

set(SOURCE_FILES
	math.cpp
)

add_library(${TARGET} STATIC ${HEADER_FILES}  ${SOURCE_FILES} ${RHI_FILES})