project(LES)

set(INCLUDE_DIRS
    ${NASSP_SOURCE_ROOT}/src_aux
    ${NASSP_SOURCE_ROOT}/src_saturn
    ${NASSP_SOURCE_ROOT}/src_sys
    ${ORBITERSDK_DIR}/include
)
set(SOURCES
    ${NASSP_SOURCE_ROOT}/src_saturn/LES.cpp
)

link_directories(${ORBITERSDK_DIR}/lib)

add_library(LES SHARED ${SOURCES})
target_include_directories(LES PRIVATE ${INCLUDE_DIRS})
target_link_libraries(LES Orbiter)
target_link_libraries(LES Orbitersdk)