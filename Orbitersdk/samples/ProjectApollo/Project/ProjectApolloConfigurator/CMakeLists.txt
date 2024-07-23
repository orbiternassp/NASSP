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

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${FINAL_INSTALL_DIR}/Modules/Startup)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_sources(ProjectApolloConfigurator PRIVATE ${NASSP_SOURCE_ROOT}/src_aux/ProjectApolloConfigurator/ProjectApolloConfigurator.rc)
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} dxguid)
target_link_libraries(${PROJECT_NAME} dinput8)
target_link_libraries(${PROJECT_NAME} GLU32)
target_link_libraries(${PROJECT_NAME} User32)
target_link_libraries(${PROJECT_NAME} Orbiter)
target_link_libraries(${PROJECT_NAME} Orbitersdk)
target_link_libraries(${PROJECT_NAME} OpenGL32)