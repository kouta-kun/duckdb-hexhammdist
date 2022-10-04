cmake_minimum_required(VERSION 2.8.12)
project(hexhammdist_extension)

set(PARAMETERS "-no-warnings")
build_loadable_extension(
  hexhammdist_extension ${PARAMETERS} hexhammdist_extension.cpp)
