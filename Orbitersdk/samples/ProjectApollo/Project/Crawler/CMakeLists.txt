project(Crawler)

set(INCLUDE_DIRS
    ${NASSP_SOURCE_ROOT}/src_aux
    ${NASSP_SOURCE_ROOT}/src_csm
    ${NASSP_SOURCE_ROOT}/src_launch
    ${NASSP_SOURCE_ROOT}/src_mfd
    ${NASSP_SOURCE_ROOT}/src_saturn
    ${NASSP_SOURCE_ROOT}/src_sys
    ${ORBITERSDK_DIR}/include
    ${ORBITERSDK_DIR}/XRSound
)
set(SOURCES
    ${NASSP_SOURCE_ROOT}/src_aux/tracer.cpp
    ${NASSP_SOURCE_ROOT}/src_launch/Crawler.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/soundlib.cpp
)

link_directories(${ORBITERSDK_DIR}/lib)
link_directories(${ORBITERSDK_DIR}/XRSound)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE
    Orbiter
    Orbitersdk
    XRSound
)