project(Sat5Abort3)

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
    ${NASSP_SOURCE_ROOT}/src_csm/smjc.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/Sat5Abort3.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/DelayTimer.cpp
)

link_directories(${ORBITERSDK_DIR}/lib)
link_directories(${ORBITERSDK_DIR}/XRSound)
link_directories(${CMAKE_BINARY_DIR}/Projects/PanelSDK)

add_library(Sat5Abort3 SHARED ${SOURCES})
target_include_directories(Sat5Abort3 PRIVATE ${INCLUDE_DIRS})
target_link_libraries(Sat5Abort3 Orbiter)
target_link_libraries(Sat5Abort3 Orbitersdk)
target_link_libraries(Sat5Abort3 PanelSDK)
target_link_libraries(Sat5Abort3 XRSound)