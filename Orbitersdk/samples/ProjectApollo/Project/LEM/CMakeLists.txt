# Set the project name
project(LEM)

set(INCLUDE_DIRS
    ${NASSP_SOURCE_ROOT}/src_aux
    ${NASSP_SOURCE_ROOT}/src_csm
    ${NASSP_SOURCE_ROOT}/src_launch
    ${NASSP_SOURCE_ROOT}/src_lm
    ${NASSP_SOURCE_ROOT}/src_lm/yaAGS
    ${NASSP_SOURCE_ROOT}/src_mfd
    ${NASSP_SOURCE_ROOT}/src_moon
    ${NASSP_SOURCE_ROOT}/src_saturn
    ${NASSP_SOURCE_ROOT}/src_sys
    ${NASSP_SOURCE_ROOT}/src_sys/PanelSDK/Internals
    ${NASSP_SOURCE_ROOT}/src_sys/yaAGC
    ${ORBITERSDK_DIR}/include
    ${ORBITERSDK_DIR}/XRSound
)
set(SOURCES
    ${NASSP_SOURCE_ROOT}/src_aux/animations.cpp
    ${NASSP_SOURCE_ROOT}/src_aux/BasicExcelVC6.cpp
    ${NASSP_SOURCE_ROOT}/src_aux/CDK.cpp
    ${NASSP_SOURCE_ROOT}/src_aux/Mission.cpp
    ${NASSP_SOURCE_ROOT}/src_aux/OrbiterMath.cpp
    ${NASSP_SOURCE_ROOT}/src_aux/tracer.cpp
    ${NASSP_SOURCE_ROOT}/src_aux/vesim.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/CWEA_FlipFlop.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/LEM.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/LEMcomputer.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/lemconnector.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/lemmesh.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/lempanel.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/lemswitches.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/lemsystems.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/lemvc.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/lm_aca.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/lm_aeaa.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/lm_ags.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/lm_aps.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/lm_cwea.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/lm_dps.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/lm_ecs.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/lm_eds.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/lm_eps.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/lm_lr.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/lm_programer.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/lm_rcs.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/lm_rr.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/lm_scea.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/lm_telecom.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/lm_ttca.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/lmscs.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/LMMalfunctionSimulation.cpp
    ${NASSP_SOURCE_ROOT}/src_lm/yaAGS/aea_engine.c
    ${NASSP_SOURCE_ROOT}/src_lm/yaAGS/aea_engine_init.c
    ${NASSP_SOURCE_ROOT}/src_lm/yaAGS/OutputAPI_AGS.c
    ${NASSP_SOURCE_ROOT}/src_mfd/MFDconnector.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/apolloguidance.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/cautionwarning.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/cdu.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/checklistController.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/checklistControllerHelpers.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/connector.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/DelayTimer.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/dsky.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/dskyinterface.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/FDAI.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/imu.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/imulog.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/imumath.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/inertial.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/Malfunction.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/MalfunctionSimulation.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/mechanicalaccelerometer.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/missiontimer.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/ordeal.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/soundevents.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/soundlib.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/payload.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/powersource.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/pyro.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/thread.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/toggleswitch.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/yaAGC/agc_engine.c
    ${NASSP_SOURCE_ROOT}/src_sys/yaAGC/agc_engine_init.c
    ${NASSP_SOURCE_ROOT}/src_sys/yaAGC/agc_utilities.c
    ${NASSP_SOURCE_ROOT}/src_sys/yaAGC/Backtrace.c
    ${NASSP_SOURCE_ROOT}/src_sys/yaAGC/random.c
    ${NASSP_SOURCE_ROOT}/src_sys/yaAGC/rfopen.c
)

link_directories(${ORBITERSDK_DIR}/lib)
link_directories(${ORBITERSDK_DIR}/XRSound)
link_directories(${CMAKE_BINARY_DIR}/Projects/PanelSDK)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_sources(LEM PRIVATE ${NASSP_SOURCE_ROOT}/src_lm/LEMResources.rc)
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} GLU32)
target_link_libraries(${PROJECT_NAME} dxguid)
target_link_libraries(${PROJECT_NAME} dinput8)
target_link_libraries(${PROJECT_NAME} OpenGL32)
target_link_libraries(${PROJECT_NAME} User32)
target_link_libraries(${PROJECT_NAME} WinMM)
target_link_libraries(${PROJECT_NAME} WS2_32)
target_link_libraries(${PROJECT_NAME} Orbiter)
target_link_libraries(${PROJECT_NAME} Orbitersdk)
target_link_libraries(${PROJECT_NAME} PanelSDK)
target_link_libraries(${PROJECT_NAME} XRSound)