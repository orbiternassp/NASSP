project(s1c)

set(INCLUDE_DIRS
    ${NASSP_SOURCE_ROOT}/src_aux
    ${NASSP_SOURCE_ROOT}/src_csm
    ${NASSP_SOURCE_ROOT}/src_launch
    ${NASSP_SOURCE_ROOT}/src_mfd
    ${NASSP_SOURCE_ROOT}/src_saturn
    ${NASSP_SOURCE_ROOT}/src_sys
    ${NASSP_SOURCE_ROOT}/src_sys/PanelSDK
    ${ORBITERSDK_DIR}/include
    ${ORBITERSDK_DIR}/XRSound
)
set(SOURCES
    ${NASSP_SOURCE_ROOT}/src_saturn/s1c.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/connector.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/pyro.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/soundlib.cpp
)

link_directories(${ORBITERSDK_DIR}/lib)
link_directories(${ORBITERSDK_DIR}/XRSound)
link_directories(${CMAKE_BINARY_DIR}/Projects/PanelSDK)

add_library(s1c SHARED ${SOURCES})
target_include_directories(s1c PRIVATE ${INCLUDE_DIRS})
target_link_libraries(s1c Orbiter)
target_link_libraries(s1c Orbitersdk)
target_link_libraries(s1c PanelSDK)
target_link_libraries(s1c XRSound)