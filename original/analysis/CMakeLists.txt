########################################################
# cmake file for building Marlin example Package
# @author Jan Engels, DESY
CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)
########################################################

# project name
PROJECT( nnhAnalysis )

# project version
SET( ${PROJECT_NAME}_VERSION_MAJOR 0 )
SET( ${PROJECT_NAME}_VERSION_MINOR 1 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )

LIST( APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake )

### DEPENDENCIES ############################################################

FIND_PACKAGE( ILCUTIL COMPONENTS ILCSOFT_CMAKE_MODULES REQUIRED )
# load default settings from ILCSOFT_CMAKE_MODULES
INCLUDE( ilcsoft_default_settings )

FIND_PACKAGE( Eigen3 REQUIRED )
INCLUDE_DIRECTORIES( SYSTEM ${EIGEN3_INCLUDE_DIR} )
LIST( APPEND ${PROJECT_NAME}_DEPENDS_INCLUDE_DIRS ${EIGEN3_INCLUDE_DIR} )

FIND_PACKAGE( ROOT 6.18 REQUIRED COMPONENTS ROOTDataFrame ROOTVecOps)

FOREACH( pkg ROOT )
    IF( ${pkg}_FOUND )
		INCLUDE_DIRECTORIES( SYSTEM ${${pkg}_INCLUDE_DIRS} )
        LINK_LIBRARIES( ${${pkg}_LIBRARIES} )
        ADD_DEFINITIONS ( ${${pkg}_DEFINITIONS} )
    ENDIF()
ENDFOREACH()

# definitions to pass to the compiler
ADD_DEFINITIONS( "-Wall -ansi -pedantic -std=c++17 -Weffc++" )
ADD_DEFINITIONS( "-Wno-long-long" )
ADD_DEFINITIONS( "-O3" )

include_directories( ${CMAKE_CURRENT_LIST_DIR}/include )

#file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.cc)
#file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/*.hh ${CMAKE_CURRENT_LIST_DIR}/include/*.hpp)

#add library
#add_library( nnhAnalysis STATIC ${SOURCES} ${HEADERS} )
#target_link_libraries( nnhAnalysis ${ROOT_COMPONENT_LIBRARIES} )
#install( TARGETS nnhAnalysis LIBRARY DESTINATION ${CMAKE_CURRENT_LIST_DIR}/lib )

file( GLOB APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/exec/*.cxx )
foreach( testsourcefile ${APP_SOURCES} )
	get_filename_component(testname ${testsourcefile} NAME_WE)
	add_executable( ${testname} ${testsourcefile} )
#	target_link_libraries( ${testname} nnhAnalysis ${ROOT_LIBRARIES} )
	install(TARGETS ${testname} RUNTIME DESTINATION ${CMAKE_CURRENT_LIST_DIR}/bin)
endforeach( testsourcefile ${APP_SOURCES} )
