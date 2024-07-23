project(ApolloRTCCMFD)

set(INCLUDE_DIRS
    ${NASSP_SOURCE_ROOT}/src_aux
    ${NASSP_SOURCE_ROOT}/src_csm
    ${NASSP_SOURCE_ROOT}/src_launch
    ${NASSP_SOURCE_ROOT}/src_lm
    ${NASSP_SOURCE_ROOT}/src_mfd
    ${NASSP_SOURCE_ROOT}/src_rtccmfd
    ${NASSP_SOURCE_ROOT}/src_saturn
    ${NASSP_SOURCE_ROOT}/src_sys
    ${ORBITERSDK_DIR}/include
)
set(SOURCES
    ${NASSP_SOURCE_ROOT}/src_launch/rtcc.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/AnalyticEphemerisGenerator.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/ApolloGeneralizedOpticsProgram.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/ApollomfdButtons.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/ApolloRTCCMFD.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/ApolloRTCCMFD_Display.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/ARCore.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/ARoapiModule.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/AscentRendezvousMonitor.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/CoastNumericalIntegrator.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/CSMLMGuidanceSim.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/EnckeIntegrator.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/EntryCalculations.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/GeneralizedIterator.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/GeneralPurposeManeuver.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/LDPP.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/LMGuidanceSim.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/LOITargeting.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/LunarTargetingProgram.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/LWP.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/OrbMech.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/ReentryNumericalIntegrator.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/RTCC_EMSMISS.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/rtcc_intermediate_library_programs.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/rtcc_library_programs.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/RTCCModule.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/RTCCDisplayFormatting.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/TLIGuidanceSim.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/TLIProcessor.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/TLMCC.cpp
    ${NASSP_SOURCE_ROOT}/src_rtccmfd/TLTrajectoryComputers.cpp
    ${NASSP_SOURCE_ROOT}/src_sys/thread.cpp
)

link_directories(${ORBITERSDK_DIR}/lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${FINAL_INSTALL_DIR}/Modules/Plugin)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Orbiter)
target_link_libraries(${PROJECT_NAME} Orbitersdk)
target_link_libraries(${PROJECT_NAME} WinMM)
target_link_libraries(${PROJECT_NAME} WS2_32)