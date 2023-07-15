#!/bin/bash
mkdir -p build
cd build
cmake -S ../ -B .

if [ -z "$MAKEFLAGS" ]
then
    export MAKEFLAGS=-j$(($(nproc)/2))
    # export MAKEFLAGS=-j$(nproc)
fi

make server
make client
cd ..