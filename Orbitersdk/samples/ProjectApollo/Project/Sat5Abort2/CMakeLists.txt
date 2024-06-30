project(Sat5Abort2)

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
    ${NASSP_SOURCE_ROOT}/src_saturn/Sat5Abort2.cpp
)

link_directories(${ORBITERSDK_DIR}/lib)
link_directories(${ORBITERSDK_DIR}/XRSound)
link_directories(${CMAKE_BINARY_DIR}/Projects/PanelSDK)

add_library(Sat5Abort2 SHARED ${SOURCES})
target_include_directories(Sat5Abort2 PRIVATE ${INCLUDE_DIRS})
target_link_libraries(Sat5Abort2 Orbiter)
target_link_libraries(Sat5Abort2 Orbitersdk)
target_link_libraries(Sat5Abort2 PanelSDK)
target_link_libraries(Sat5Abort2 XRSound)