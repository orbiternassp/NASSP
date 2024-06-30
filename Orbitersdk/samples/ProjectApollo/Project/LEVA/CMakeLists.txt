project(LEVA)

set(INCLUDE_DIRS
    ${NASSP_SOURCE_ROOT}/src_aux
    ${NASSP_SOURCE_ROOT}/src_csm
    ${NASSP_SOURCE_ROOT}/src_lm
    ${NASSP_SOURCE_ROOT}/src_mfd
    ${NASSP_SOURCE_ROOT}/src_moon
    ${NASSP_SOURCE_ROOT}/src_sys
    ${ORBITERSDK_DIR}/include
    ${ORBITERSDK_DIR}/XRSound
)
set(SOURCES
    ${NASSP_SOURCE_ROOT}/src_aux/CDK.cpp
    ${NASSP_SOURCE_ROOT}/src_aux/OrbiterMath.cpp
    ${NASSP_SOURCE_ROOT}/src_aux/tracer.cpp
    ${NASSP_SOURCE_ROOT}/src_moon/leva.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/soundlib.cpp
)

link_directories(${ORBITERSDK_DIR}/lib)
link_directories(${ORBITERSDK_DIR}/XRSound)

add_library(LEVA SHARED ${SOURCES})
target_include_directories(LEVA PRIVATE ${INCLUDE_DIRS})
target_link_libraries(LEVA Orbiter)
target_link_libraries(LEVA Orbitersdk)
target_link_libraries(LEVA XRSound)