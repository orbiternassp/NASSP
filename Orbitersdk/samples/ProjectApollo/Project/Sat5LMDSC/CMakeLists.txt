project(Sat5LMDSC)

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
    ${NASSP_SOURCE_ROOT}/src_aux/Mission.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/Sat5LMDSC.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/soundlib.cpp
)

link_directories(${ORBITERSDK_DIR}/lib)
link_directories(${ORBITERSDK_DIR}/XRSound)
link_directories(${CMAKE_BINARY_DIR}/Projects/PanelSDK)

add_library(Sat5LMDSC SHARED ${SOURCES})
target_include_directories(Sat5LMDSC PRIVATE ${INCLUDE_DIRS})
target_link_libraries(Sat5LMDSC Orbiter)
target_link_libraries(Sat5LMDSC Orbitersdk)
target_link_libraries(Sat5LMDSC PanelSDK)
target_link_libraries(Sat5LMDSC XRSound)