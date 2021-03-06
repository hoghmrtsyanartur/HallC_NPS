#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(glass)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE})

#----------------------------------------------------------------------------
# Find ROOT (required package)
# Petr Stepanov: Find ROOT with RooFit https://root.cern/manual/integrate_root_into_my_cmake_project/
#
find_package(ROOT REQUIRED COMPONENTS RooFit RooFitCore)

# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include 
#                   ${PROJECT_SOURCE_DIR}/shared/include 
                    ${Geant4_INCLUDE_DIR}
                    ${ROOT_INCLUDE_DIR})
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc
#                 ${PROJECT_SOURCE_DIR}/shared/src/*.cc
                  )
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh
#                 ${PROJECT_SOURCE_DIR}/shared/include/*.hh
                  )

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(glass glass.cc ${sources} ${headers})
target_link_libraries(glass ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} )

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build glass. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(glass_MACROS
    /macros/i.mac /macros/farm.mac /macros/farm_test_op.mac /macros/vis.mac /macros/test.mac /macros/interactive_test_op.mac /macros/interactive_test_op_cs137.mac /macros/batch.mac
  )

foreach(_macros ${glass_MACROS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_macros}
    ${PROJECT_BINARY_DIR}/${_macros}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# Copy ROOT scripts that plot the Geant data to the build directory

set(glass_SCRIPTS
    /draw/energy-resolution/energyResolution.cpp /draw/energy-resolution/energyResolution.gp
  )

foreach(_file ${glass_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_file}
    ${PROJECT_BINARY_DIR}/${_file}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS glass DESTINATION bin)

