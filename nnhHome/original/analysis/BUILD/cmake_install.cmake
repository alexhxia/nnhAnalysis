# Install script for directory: /home/ilc/ahocine/nnhAnalysis/nnhHome/original/analysis

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/ilc/ahocine/nnhAnalysis/nnhHome/original/analysis")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/home/ilc/ahocine/nnhAnalysis/nnhHome/original/analysis/bin/prepareForBDT" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/ilc/ahocine/nnhAnalysis/nnhHome/original/analysis/bin/prepareForBDT")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/ilc/ahocine/nnhAnalysis/nnhHome/original/analysis/bin/prepareForBDT"
         RPATH "/home/ilc/ahocine/nnhAnalysis/nnhHome/original/analysis/lib:/cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/root/6.18.04/lib")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/ilc/ahocine/nnhAnalysis/nnhHome/original/analysis/bin/prepareForBDT")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/ilc/ahocine/nnhAnalysis/nnhHome/original/analysis/bin" TYPE EXECUTABLE FILES "/home/ilc/ahocine/nnhAnalysis/nnhHome/original/analysis/BUILD/bin/prepareForBDT")
  if(EXISTS "$ENV{DESTDIR}/home/ilc/ahocine/nnhAnalysis/nnhHome/original/analysis/bin/prepareForBDT" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/ilc/ahocine/nnhAnalysis/nnhHome/original/analysis/bin/prepareForBDT")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/home/ilc/ahocine/nnhAnalysis/nnhHome/original/analysis/bin/prepareForBDT"
         OLD_RPATH "/cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/root/6.18.04/lib::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
         NEW_RPATH "/home/ilc/ahocine/nnhAnalysis/nnhHome/original/analysis/lib:/cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/root/6.18.04/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/cvmfs/sft.cern.ch/lcg/releases/binutils/2.30-e5b21/x86_64-centos7/bin/strip" "$ENV{DESTDIR}/home/ilc/ahocine/nnhAnalysis/nnhHome/original/analysis/bin/prepareForBDT")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/ilc/ahocine/nnhAnalysis/nnhHome/original/analysis/BUILD/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
