cmake_minimum_required(VERSION 2.8.12)
project(hexhammdist_extension)

set(IGNORE_WARNINGS OFF)

build_loadable_extension(
  hexhammdist_extension hexhammdist_extension.cpp)
