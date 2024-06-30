project(ASTP)

set(INCLUDE_DIRS
    ${NASSP_SOURCE_ROOT}/src_saturn
    ${NASSP_SOURCE_ROOT}/src_sys
    ${ORBITERSDK_DIR}/include
)
set(SOURCES
    ${NASSP_SOURCE_ROOT}/src_saturn/ASTP.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/connector.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/payload.cpp
)

link_directories(${ORBITERSDK_DIR}/lib)
link_directories(${ORBITERSDK_DIR}/XRSound)
link_directories(${CMAKE_BINARY_DIR}/Projects/PanelSDK)

add_library(ASTP SHARED ${SOURCES})
target_include_directories(ASTP PRIVATE ${INCLUDE_DIRS})
target_link_libraries(ASTP Orbiter)
target_link_libraries(ASTP Orbitersdk)
target_link_libraries(ASTP PanelSDK)