#!/bin/bash

set -e

if [[ "$#" -ne 1 ]]; then
    echo "Invalid number of arguments."
    exit 1
fi

rm -fr ./simulation-build/ &&
mkdir ./simulation-build/ &&
cd ./simulation-build/ &&
cmake ../simulation &&
make &&
cd .. &&
./simulation-build/simulation "$1"
