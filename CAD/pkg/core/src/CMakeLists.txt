project(PM_ATPG)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
SET(CMAKE_C_FLAGS "-O2 -Wall -g -fopenmp  -DUSE_OPENMP")
SET(CMAKE_CXX_FLAGS "-O2 -Wall -g -fopenmp  -DUSE_OPENMP  -std=gnu++0x")
add_executable(PM_ATPG
atpg.h atpg.cpp
circuit.h circuit.cpp
clause.h clause.cpp
fault.h fault.cpp
gate.h gate.cpp
main.cpp
path.h path.cpp
sat.h sat.cpp
spec.h spec.cpp
util.h util.cpp
wire.h wire.cpp
)

