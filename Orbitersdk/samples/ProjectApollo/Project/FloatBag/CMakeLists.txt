project(FloatBag)

set(INCLUDE_DIRS
    ${NASSP_SOURCE_ROOT}/src_aux
    ${NASSP_SOURCE_ROOT}/src_landing
    ${NASSP_SOURCE_ROOT}/src_sys
    ${ORBITERSDK_DIR}/include
)
set(SOURCES
    ${NASSP_SOURCE_ROOT}/src_aux/tracer.cpp
    ${NASSP_SOURCE_ROOT}/src_landing/FloatBag.cpp
)

link_directories(${ORBITERSDK_DIR}/lib)

add_library(FloatBag SHARED ${SOURCES})
target_include_directories(FloatBag PRIVATE ${INCLUDE_DIRS})
target_link_libraries(FloatBag Orbiter)
target_link_libraries(FloatBag Orbitersdk)