project(sm)

set(INCLUDE_DIRS
    ${NASSP_SOURCE_ROOT}/src_aux
    ${NASSP_SOURCE_ROOT}/src_csm
    ${NASSP_SOURCE_ROOT}/src_sys
    ${NASSP_SOURCE_ROOT}/src_sys/PanelSDK
    ${ORBITERSDK_DIR}/include
    ${ORBITERSDK_DIR}/XRSound
)
set(SOURCES
    ${NASSP_SOURCE_ROOT}/src_csm/smjc.cpp
    ${NASSP_SOURCE_ROOT}/src_csm/sm.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/DelayTimer.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/soundlib.cpp
)

link_directories(${ORBITERSDK_DIR}/lib)
link_directories(${ORBITERSDK_DIR}/XRSound)
link_directories(${CMAKE_BINARY_DIR}/Projects/PanelSDK)

add_library(sm SHARED ${SOURCES})
target_include_directories(sm PRIVATE ${INCLUDE_DIRS})
target_link_libraries(sm Orbiter)
target_link_libraries(sm Orbitersdk)
target_link_libraries(sm PanelSDK)
target_link_libraries(sm XRSound)