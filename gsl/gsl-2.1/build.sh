#!/bin/sh
mkdir -p build
cd build
export PATH=`pwd`/../../wllvm/wllvm:$PATH
CC=wllvm ./../configure --prefix=`pwd`/../install
make -j 2
make install
cd install/lib
extract-bc libgsl.a
extract-bc libgslcblas.a
cd ../..
