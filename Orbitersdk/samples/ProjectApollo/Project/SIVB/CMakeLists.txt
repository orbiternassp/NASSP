project(SIVB)

set(INCLUDE_DIRS
    ${NASSP_SOURCE_ROOT}/src_aux
    ${NASSP_SOURCE_ROOT}/src_csm
    ${NASSP_SOURCE_ROOT}/src_launch
    ${NASSP_SOURCE_ROOT}/src_lm
    ${NASSP_SOURCE_ROOT}/src_mfd
    ${NASSP_SOURCE_ROOT}/src_saturn
    ${NASSP_SOURCE_ROOT}/src_sys
    ${NASSP_SOURCE_ROOT}/src_sys/PanelSDK
    ${ORBITERSDK_DIR}/include
    ${ORBITERSDK_DIR}/XRSound
)
set(SOURCES
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/OrbMech.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/dcs.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/eds.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/FCC.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/iu.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/IUAuxiliaryPowerDistributor.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/IUControlDistributor.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/IUControlSignalProcessor.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/LVDA.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/LVDC.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/lvimu.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/sivb.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/sivbsystems.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/connector.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/DelayTimer.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/inertial.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/powersource.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/pyro.cpp
)

link_directories(${ORBITERSDK_DIR}/lib)
link_directories(${ORBITERSDK_DIR}/XRSound)
link_directories(${CMAKE_BINARY_DIR}/Projects/PanelSDK)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE
    Orbiter
    Orbitersdk
    PanelSDK
    XRSound
)