#! /usr/bin/env bash
export MARLIN=/cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/Marlin/v01-17-01

export PATH=${MARLIN}/bin:${PATH}

export GSL_HOME=/cvmfs/ilc.desy.de/sw/x86_64_gcc82_centos7/v02-02-03/gsl/2.6

export PATH=${GSL_HOME}/bin:${PATH}

export LD_LIBRARY_PATH=/cvmfs/sft.cern.ch/lcg/releases/gcc/8.2.0-3fa06/x86_64-centos7/lib64:/cvmfs/sft.cern.ch/lcg/releases/gcc/8.2.0-3fa06/x86_64-centos7/lib:${LD_LIBRARY_PATH}

export LD_LIBRARY_PATH=${GSL_HOME}/lib:${LD_LIBRARY_PATH}