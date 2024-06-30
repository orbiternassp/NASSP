project(ProjectApolloConfigurator)

set(INCLUDE_DIRS
    ${NASSP_SOURCE_ROOT}/src_aux
    ${NASSP_SOURCE_ROOT}/src_aux/ProjectApolloConfigurator
    ${ORBITERSDK_DIR}/include
)
set(SOURCES
    ${NASSP_SOURCE_ROOT}/src_aux/vesim.cpp
    ${NASSP_SOURCE_ROOT}/src_aux/ProjectApolloConfigurator/ProjectApolloConfigurator.cpp
)

link_directories(${ORBITERSDK_DIR}/lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ORBITER_INSTALL_DIR}/Modules/Startup)

add_library(ProjectApolloConfigurator SHARED ${SOURCES})
target_sources(ProjectApolloConfigurator PRIVATE ${NASSP_SOURCE_ROOT}/src_aux/ProjectApolloConfigurator/ProjectApolloConfigurator.rc)
target_include_directories(ProjectApolloConfigurator PRIVATE ${INCLUDE_DIRS})
target_link_libraries(ProjectApolloConfigurator dxguid)
target_link_libraries(ProjectApolloConfigurator dinput8)
target_link_libraries(ProjectApolloConfigurator GLU32)
target_link_libraries(ProjectApolloConfigurator User32)
target_link_libraries(ProjectApolloConfigurator Orbiter)
target_link_libraries(ProjectApolloConfigurator Orbitersdk)
target_link_libraries(ProjectApolloConfigurator OpenGL32)