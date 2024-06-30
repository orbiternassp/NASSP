cmake_minimum_required(VERSION 3.19)

# Set the project name
project(NASSP VERSION 8.0.3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Add /permissive if using C++20 or higher
#add_compile_options(/permissive)

# Group projects in VS
#set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Check if the user has specified a non-default Orbiter installation location.
# If not, use the default and alert the user.
set(ORBITER_INSTALL_DIR ${CMAKE_SOURCE_DIR}/../../../ CACHE STRING "Location of OpenOrbiter installation directory")

if(CMAKE_SIZEOF_VOID_P EQUAL 8) # 64-bit build?
    set(BUILD64 1)
    set(ARCH "x64")
    set(PLATFORM_STRING "x64")
else()
    set(BUILD64 0)
    set(ARCH "x86")
    set(PLATFORM_STRING "Win32")
endif()

if (NOT ${CMAKE_GENERATOR} STREQUAL "Ninja")
    add_compile_options(/MP)
endif()

if(MSVC)
    # Flag pointer truncation as an error because if it happens that's bad news!
    add_compile_options(/we4311)
    #add_link_options("/NODEFAULTLIB:\"LIBCMTD\"")
endif()

# Set project-wide variables
set(ORBITERSDK_DIR ${ORBITER_INSTALL_DIR}/Orbitersdk)
set(NASSP_SOURCE_ROOT ${CMAKE_SOURCE_DIR})

# Set project libraries to output their DLL files and debug info to
# the Orbiter installation's "Modules/ProjectApollo" directory,
# unless a sub-project's CMakeLists.txt file overrides this locally,
# such as MFDs, which need to go in "Modules/Plugins" or PanelSDK,
# which can stay in the default directory since it's statically linked.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ORBITER_INSTALL_DIR}/Modules/ProjectApollo)

# Sub-projects are what actually build all our vessels, MFDs, etc.
add_subdirectory(Project/PanelSDK)
add_subdirectory(Project/ApolloRTCCMFD)
add_subdirectory(Project/ASTP)
add_subdirectory(Project/CMChute)
add_subdirectory(Project/Crawler)
add_subdirectory(Project/LEM)
add_subdirectory(Project/ProjectApolloConfigurator)
add_subdirectory(Project/ProjectApolloMFD)
add_subdirectory(Project/Saturn1B)
add_subdirectory(Project/Saturn5)