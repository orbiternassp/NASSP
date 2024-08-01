cmake_minimum_required(VERSION 3.19)

# Set the project name
project(NASSP VERSION 8.0.3)

# Set C++ compiler variables
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

# Microsoft Visual C compiler specific options
if(MSVC)
    # Flag pointer truncation as an error because if it happens that's bad news!
    add_compile_options("/we4311")
    # We need to force the linker to compile us against the statically-linked
    # C runtime when building for Orbiter Beta due to XRSound.
    if(NOT OPENORBITER)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        #add_compile_options("/MT")
    endif()
    add_link_options("/NODEFAULTLIB:LIBCMTD")
endif()

#Add /permissive if using C++20 or higher
#add_compile_options(/permissive)

# Group projects in VS
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set NASSP-related variables
set(ORBITERSDK_DIR ${ORBITER_INSTALL_DIR}/Orbitersdk)
set(NASSP_SOURCE_ROOT ${CMAKE_SOURCE_DIR})
set(NASSP_REPO_ROOT ${NASSP_SOURCE_ROOT}/../../../)
option(OPENORBITER OFF)

# Check if the user has specified a non-default Orbiter installation location.
# If not, use the default and alert the user.
set(ORBITER_INSTALL_DIR ${NASSP_REPO_ROOT} CACHE STRING "Location of OpenOrbiter installation directory")

# Allow having a separate installation directory than the Orbiter installation folder.
# This is generally meant for automated build systems or for folks who want to produce 
# an installation package of just NASSP's data and modules without
# including Orbiter data or any NASSP git data or source files.
set(FINAL_INSTALL_DIR ${ORBITER_INSTALL_DIR} CACHE STRING "Location for final build/file copy output, defaults to your chosen Orbiter install directory")

# Set project libraries to output their DLL files and debug info to
# the final installation dir's "Modules/ProjectApollo" directory,
# unless a sub-project's CMakeLists.txt file overrides this locally,
# such as MFDs, which need to go in "Modules/Plugins" or PanelSDK,
# which can stay in the default directory since it's statically linked.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${FINAL_INSTALL_DIR}/Modules/ProjectApollo)

# Sub-projects are what actually build all our vessels, MFDs, etc.
add_subdirectory(Project/PanelSDK)
add_subdirectory(Project/ApolloRTCCMFD)
add_subdirectory(Project/ASTP)
add_subdirectory(Project/CMChute)
add_subdirectory(Project/Crawler)
add_subdirectory(Project/EVA)
add_subdirectory(Project/FloatBag)
add_subdirectory(Project/Floodlight)
add_subdirectory(Project/LC34)
add_subdirectory(Project/LC37)
add_subdirectory(Project/LCC)
add_subdirectory(Project/LEM)
add_subdirectory(Project/LES)
add_subdirectory(Project/LEVA)
add_subdirectory(Project/LRV)
add_subdirectory(Project/MCC)
add_subdirectory(Project/ML)
add_subdirectory(Project/MSS)
add_subdirectory(Project/ProjectApolloConfigurator)
add_subdirectory(Project/ProjectApolloMFD)
add_subdirectory(Project/Saturn1B)
add_subdirectory(Project/Saturn5)
add_subdirectory(Project/s1b)
add_subdirectory(Project/s1c)
add_subdirectory(Project/Sat1bAbort1)
add_subdirectory(Project/Sat1bAbort2)
add_subdirectory(Project/Sat5Abort1)
add_subdirectory(Project/Sat5Abort2)
add_subdirectory(Project/Sat5Abort3)
add_subdirectory(Project/Sat5LMDSC)
add_subdirectory(Project/sii)
add_subdirectory(Project/SIVB)
add_subdirectory(Project/Skylab)
add_subdirectory(Project/sm)
add_subdirectory(Project/VAB)


# Copy all relevant documents/scenarios/sounds/configs from this
# repository into the configured final destination directory.
# This will only happen if the NASSP repo is NOT located inside
# the Orbiter installation directory. If we are located
# inside an Orbiter installation, we can't easily differentiate
# between our own files and those included with Orbiter.
if(${NASSP_REPO_ROOT} STREQUAL ${ORBITER_INSTALL_DIR})
    message("[NASSP] Refusing to copy config files, NASSP is cloned directly into an Orbiter installation.")
elseif(${NASSP_REPO_ROOT} STREQUAL ${FINAL_INSTALL_DIR})
    message("[NASSP] Unnecessary to copy config files, final output directory is the NASSP repo location.")
else()
    message("[NASSP] Copying config/data files to final destination directory, please wait...")

    # Create the list of input directories to be copied to the install location
    set(NASSP_INSTALL_DIR_LIST
        "Config"
        "Doc"
        "Html"
        "Meshes"
        "Scenarios"
        "Script"
        "Sound"
        "Textures"
        "XRSound"
    )

    foreach(dir ${NASSP_INSTALL_DIR_LIST})
        # Create the top-level folders.
        #message("[NASSP] Making directory: ${FINAL_INSTALL_DIR}/${dir}")
        file(MAKE_DIRECTORY ${FINAL_INSTALL_DIR}/${dir})

        # Recursively grab a list of directories/files.
        file(GLOB_RECURSE STUFF_TO_COPY LIST_DIRECTORIES true ${NASSP_REPO_ROOT}/${dir}/*)

        # First pass, create only the entries which are directories, in the output destination.
        foreach(entry ${STUFF_TO_COPY})
            if(IS_DIRECTORY ${entry})
                file(RELATIVE_PATH nested_dir ${NASSP_REPO_ROOT} ${entry})
                #message("[NASSP] Making subdirectory: ${FINAL_INSTALL_DIR}/${nested_dir}")
                file(MAKE_DIRECTORY ${FINAL_INSTALL_DIR}/${nested_dir})
            endif()
        endforeach()

        # Second pass, now copy the files.
        foreach(entry ${STUFF_TO_COPY})
            if(NOT IS_DIRECTORY ${entry})
                file(RELATIVE_PATH file ${NASSP_REPO_ROOT} ${entry})
                #message("[NASSP] Copying file: ${FINAL_INSTALL_DIR}/${file}")
                file(COPY_FILE ${entry} ${FINAL_INSTALL_DIR}/${file} ONLY_IF_DIFFERENT)
            endif()
        endforeach()
    endforeach()

    message("[NASSP] Copying completed!")
endif()
