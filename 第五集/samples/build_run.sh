#!/bin/bash
cd "$(dirname "$0")"
cmake -B build -DCMAKE_CXX_STANDARD=20
cmake --build build
./build/run_all
