project(PanelSDK)

set(INCLUDE_DIRS
    ${NASSP_SOURCE_ROOT}/src_sys
    ${NASSP_SOURCE_ROOT}/src_sys/PanelSDK
    ${NASSP_SOURCE_ROOT}/src_sys/PanelSDK/Internals
    ${ORBITERSDK_DIR}/include
)
set(SOURCES
    ${NASSP_SOURCE_ROOT}/src_sys/PanelSDK/PanelSDK.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/PanelSDK/BUILD.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/PanelSDK/Matrix.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/PanelSDK/Vectors.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/PanelSDK/VSMGMT.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/PanelSDK/Internals/esysparse.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/PanelSDK/Internals/esystems.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/PanelSDK/Internals/hsysparse.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/PanelSDK/Internals/hsystems.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/PanelSDK/Internals/thermal.cpp
)

link_directories(${ORBITERSDK_DIR}/lib)
link_directories(${ORBITERSDK_DIR}/XRSound)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Orbiter)
target_link_libraries(${PROJECT_NAME} Orbitersdk)
target_link_libraries(${PROJECT_NAME} XRSound)