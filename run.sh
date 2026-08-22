#!/bin/bash

cd build || exit
cmake ..
make -j "$(nproc)" || exit
clear
./Deformerz
