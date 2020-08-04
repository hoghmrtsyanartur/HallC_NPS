#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(DVCS)

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
#
find_package(ROOT REQUIRED)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include 
                    ${PROJECT_SOURCE_DIR}/shared/include 
                    /pbs/throng/clas/hosanko/DVCS/dvcs_gen
                    /pbs/throng/clas/hosanko/NPS/NPS_SOFT
                    /pbs/throng/clas/hosanko/DVCS/RFunction
                    ${Geant4_INCLUDE_DIR}
                    ${ROOT_INCLUDE_DIR})
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc
                  ${PROJECT_SOURCE_DIR}/shared/src/*.cc
)

file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh
                  ${PROJECT_SOURCE_DIR}/shared/include/*.hh
)

#----------------------------------------------------------------------------
#Bring the shared libraries
#
set(TGenGeo libTGenGeo.so)
set(TGenBase libTGenBase.so)
set(TGenDVCS libTGenDVCS.so)
set(TGVKelly libTGVKelly.so)
set(TGenDeltPi0 libTGenDeltPi0.so)
set(TGenElas libTGenElas.so)
set(TGenOme libTGenOme.so)
set(TGenPi0 libTGenPi0.so)
set(TGenPip libTGenPip.so)
set(TGenRho libTGenRho.so)
link_directories(/pbs/throng/clas/hosanko/DVCS/dvcs_gen)

#For software reconstruction
set(libDVCS libDVCS.so)
link_directories(/pbs/throng/clas/hosanko/NPS/NPS_SOFT)


#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(DVCS DVCS.cc ${sources} ${headers})
target_link_libraries(DVCS ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} 
                           ${TGenGeo} ${TGenBase} ${TGenDVCS} ${TGVKelly} ${TGenDeltPi0} ${TGenElas} ${TGenOme} ${TGenPi0} ${TGenPip} ${TGenRho}
                           ${libDVCS} 
)

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build DVCS. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(DVCS_SCRIPTS
    ./macros/vis.mac ./macros/test.mac
  )

foreach(_script ${DVCS_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS DVCS DESTINATION bin)
