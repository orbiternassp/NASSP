project(EVA)

set(INCLUDE_DIRS
    ${NASSP_SOURCE_ROOT}/src_aux
    ${NASSP_SOURCE_ROOT}/src_csm
    ${NASSP_SOURCE_ROOT}/src_sys
    ${ORBITERSDK_DIR}/include
)
set(SOURCES
    ${NASSP_SOURCE_ROOT}/src_aux/tracer.cpp
    ${NASSP_SOURCE_ROOT}/src_csm/eva.cpp
)

link_directories(${ORBITERSDK_DIR}/lib)

add_library(EVA SHARED ${SOURCES})
target_include_directories(EVA PRIVATE ${INCLUDE_DIRS})
target_link_libraries(EVA Orbiter)
target_link_libraries(EVA Orbitersdk)