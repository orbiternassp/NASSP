project(Sat1bAbort1)

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
    ${NASSP_SOURCE_ROOT}/src_saturn/Saturn1Abort.cpp
)

link_directories(${ORBITERSDK_DIR}/lib)
link_directories(${ORBITERSDK_DIR}/XRSound)
link_directories(${CMAKE_BINARY_DIR}/Projects/PanelSDK)

add_library(Sat1bAbort1 SHARED ${SOURCES})
target_include_directories(Sat1bAbort1 PRIVATE ${INCLUDE_DIRS})
target_link_libraries(Sat1bAbort1 Orbiter)
target_link_libraries(Sat1bAbort1 Orbitersdk)
target_link_libraries(Sat1bAbort1 PanelSDK)
target_link_libraries(Sat1bAbort1 XRSound)