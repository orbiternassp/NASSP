project(CMChute)

set(INCLUDE_DIRS
    ${NASSP_SOURCE_ROOT}/src_aux
    ${NASSP_SOURCE_ROOT}/src_landing
    ${NASSP_SOURCE_ROOT}/src_sys
    ${ORBITERSDK_DIR}/include
)
set(SOURCES
    ${NASSP_SOURCE_ROOT}/src_aux/tracer.cpp
    ${NASSP_SOURCE_ROOT}/src_landing/CMChute.cpp
)

link_directories(${ORBITERSDK_DIR}/lib)

add_library(CMChute SHARED ${SOURCES})
target_include_directories(CMChute PRIVATE ${INCLUDE_DIRS})
target_link_libraries(CMChute Orbiter)
target_link_libraries(CMChute Orbitersdk)