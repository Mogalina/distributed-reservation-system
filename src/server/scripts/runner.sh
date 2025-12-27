#!/bin/bash

set -e

if [[ "$#" -ne 0 ]]; then
    echo "Invalid number of arguments."
    exit 1
fi

rm -fr ./build/ &&
mkdir ./build/ &&
cd ./build/ &&
cmake .. &&
make &&
cd .. &&
./build/runner
