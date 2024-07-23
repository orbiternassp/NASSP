project(Saturn5)

set(INCLUDE_DIRS
    ${NASSP_SOURCE_ROOT}/src_aux
    ${NASSP_SOURCE_ROOT}/src_csm
    ${NASSP_SOURCE_ROOT}/src_landing
    ${NASSP_SOURCE_ROOT}/src_launch
    ${NASSP_SOURCE_ROOT}/src_lm
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
    ${NASSP_SOURCE_ROOT}/src_aux/CueCard.cpp
    ${NASSP_SOURCE_ROOT}/src_aux/CueCardLocation.cpp
    ${NASSP_SOURCE_ROOT}/src_aux/CueCardManager.cpp
    ${NASSP_SOURCE_ROOT}/src_aux/Mission.cpp
    ${NASSP_SOURCE_ROOT}/src_aux/OrbiterMath.cpp
    ${NASSP_SOURCE_ROOT}/src_aux/tracer.cpp
    ${NASSP_SOURCE_ROOT}/src_aux/vesim.cpp
    ${NASSP_SOURCE_ROOT}/src_csm/csm_telecom.cpp
    ${NASSP_SOURCE_ROOT}/src_csm/csmcautionwarning.cpp
    ${NASSP_SOURCE_ROOT}/src_csm/CSMcomputer.cpp
    ${NASSP_SOURCE_ROOT}/src_csm/csmconnector.cpp
    ${NASSP_SOURCE_ROOT}/src_csm/CSMMalfunctionSimulation.cpp
    ${NASSP_SOURCE_ROOT}/src_csm/csmrcs.cpp
    ${NASSP_SOURCE_ROOT}/src_csm/csmsensors.cpp
    ${NASSP_SOURCE_ROOT}/src_csm/dockingprobe.cpp
    ${NASSP_SOURCE_ROOT}/src_csm/ecs.cpp
    ${NASSP_SOURCE_ROOT}/src_csm/eps.cpp
    ${NASSP_SOURCE_ROOT}/src_csm/rhc.cpp
    ${NASSP_SOURCE_ROOT}/src_csm/sce.cpp
    ${NASSP_SOURCE_ROOT}/src_csm/scs.cpp
    ${NASSP_SOURCE_ROOT}/src_csm/saturn.cpp
    ${NASSP_SOURCE_ROOT}/src_csm/satswitches.cpp
    ${NASSP_SOURCE_ROOT}/src_csm/satsystems.cpp
    ${NASSP_SOURCE_ROOT}/src_csm/saturnmesh.cpp
    ${NASSP_SOURCE_ROOT}/src_csm/saturnpanel.cpp
    ${NASSP_SOURCE_ROOT}/src_csm/saturnvc.cpp
    ${NASSP_SOURCE_ROOT}/src_csm/secs.cpp
    ${NASSP_SOURCE_ROOT}/src_csm/smjc.cpp
    ${NASSP_SOURCE_ROOT}/src_csm/sps.cpp
    ${NASSP_SOURCE_ROOT}/src_mfd/MFDconnector.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/OrbMech.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/saturn5.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/canard.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/dcs.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/eds.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/FCC.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/iu.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/IUAuxiliaryPowerDistributor.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/IUControlDistributor.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/IUControlSignalProcessor.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/j2engine.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/LVDA.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/LVDC.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/lvimu.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/qball.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/s1csystems.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/sat5mesh.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/siisystems.cpp
    ${NASSP_SOURCE_ROOT}/src_saturn/sivbsystems.cpp
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
link_directories(${ORBITERSDK_DIR}/lib/Lua)
link_directories(${ORBITERSDK_DIR}/..)
link_directories(${ORBITERSDK_DIR}/XRSound)
link_directories(${CMAKE_BINARY_DIR}/Projects/PanelSDK)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_sources(Saturn5 PRIVATE ${NASSP_SOURCE_ROOT}/src_csm/Resources.rc)
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} GLU32)
target_link_libraries(${PROJECT_NAME} dxguid)
target_link_libraries(${PROJECT_NAME} dinput8)
target_link_libraries(${PROJECT_NAME} OpenGL32)
target_link_libraries(${PROJECT_NAME} User32)
target_link_libraries(${PROJECT_NAME} WinMM)
target_link_libraries(${PROJECT_NAME} WS2_32)
if(OPENORBITER)
    target_link_libraries(${PROJECT_NAME} lua)
else()
    target_link_libraries(${PROJECT_NAME} lua5.1)
endif()
target_link_libraries(${PROJECT_NAME} Orbiter)
target_link_libraries(${PROJECT_NAME} Orbitersdk)
target_link_libraries(${PROJECT_NAME} PanelSDK)
target_link_libraries(${PROJECT_NAME} XRSound)