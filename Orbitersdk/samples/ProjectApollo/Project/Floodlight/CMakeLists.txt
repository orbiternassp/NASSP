project(Floodlight)

set(INCLUDE_DIRS
    ${NASSP_SOURCE_ROOT}/src_aux
    ${NASSP_SOURCE_ROOT}/src_csm
    ${NASSP_SOURCE_ROOT}/src_launch
    ${NASSP_SOURCE_ROOT}/src_sys
    ${ORBITERSDK_DIR}/include
    ${ORBITERSDK_DIR}/XRSound
)
set(SOURCES
    ${NASSP_SOURCE_ROOT}/src_aux/tracer.cpp
    ${NASSP_SOURCE_ROOT}/src_launch/Floodlight.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/soundlib.cpp
)

link_directories(${ORBITERSDK_DIR}/lib)
link_directories(${ORBITERSDK_DIR}/XRSound)

add_library(Floodlight SHARED ${SOURCES})
target_include_directories(Floodlight PRIVATE ${INCLUDE_DIRS})
target_link_libraries(Floodlight Orbiter)
target_link_libraries(Floodlight Orbitersdk)
target_link_libraries(Floodlight XRSound)