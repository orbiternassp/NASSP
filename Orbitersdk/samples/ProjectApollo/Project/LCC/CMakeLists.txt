project(LCC)

set(INCLUDE_DIRS
    ${NASSP_SOURCE_ROOT}/src_aux
    ${NASSP_SOURCE_ROOT}/src_launch
    ${NASSP_SOURCE_ROOT}/src_mfd
    ${NASSP_SOURCE_ROOT}/src_rtccmfd
    ${NASSP_SOURCE_ROOT}/src_saturn
    ${NASSP_SOURCE_ROOT}/src_sys
    ${ORBITERSDK_DIR}/include
)
set(SOURCES
    ${NASSP_SOURCE_ROOT}/src_launch/LCC.cpp
    ${NASSP_SOURCE_ROOT}/src_launch/LCC_MFD.cpp
    ${NASSP_SOURCE_ROOT}/src_launch/LCCMFDButtons.cpp
    ${NASSP_SOURCE_ROOT}/src_launch/RCA110A.cpp
)

link_directories(${ORBITERSDK_DIR}/lib)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE
    Orbiter
    Orbitersdk
)