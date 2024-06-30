project(ProjectApolloMFD)

set(INCLUDE_DIRS
    ${NASSP_SOURCE_ROOT}/src_aux
    ${NASSP_SOURCE_ROOT}/src_csm
    ${NASSP_SOURCE_ROOT}/src_mfd
    ${NASSP_SOURCE_ROOT}/src_launch
    ${NASSP_SOURCE_ROOT}/src_lm
    ${NASSP_SOURCE_ROOT}/src_rtccmfd
    ${NASSP_SOURCE_ROOT}/src_saturn
    ${NASSP_SOURCE_ROOT}/src_sys
    ${ORBITERSDK_DIR}/include
)
set(SOURCES
    ${NASSP_SOURCE_ROOT}/src_mfd/MFDConnector.cpp
    ${NASSP_SOURCE_ROOT}/src_mfd/ProjectApolloChecklistMFD.cpp
    ${NASSP_SOURCE_ROOT}/src_mfd/ProjectApolloMFD.cpp
    ${NASSP_SOURCE_ROOT}/src_mfd/ProjectApolloMFDButtons.cpp
    ${NASSP_SOURCE_ROOT}/src_mfd/ProjectApolloPlugin.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/OrbMech.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/connector.cpp
)

link_directories(${ORBITERSDK_DIR}/lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ORBITER_INSTALL_DIR}/Modules/Plugin)

add_library(ProjectApolloMFD SHARED ${SOURCES})
target_sources(ProjectApolloMFD PRIVATE ${NASSP_SOURCE_ROOT}/src_mfd/MFDResources.rc)
target_include_directories(ProjectApolloMFD PRIVATE ${INCLUDE_DIRS})
target_link_libraries(ProjectApolloMFD User32)
target_link_libraries(ProjectApolloMFD Orbiter)
target_link_libraries(ProjectApolloMFD Orbitersdk)
target_link_libraries(ProjectApolloMFD WinMM)
target_link_libraries(ProjectApolloMFD WS2_32)