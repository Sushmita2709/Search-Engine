#!/bin/bash

set -eux

mkdir -p build
cmake -Hsearch_engine -Bbuild -GNinja -DCMAKE_BUILD_TYPE=RelWithDebInfo "-DCMAKE_C_FLAGS=-Wall -Werror" "-DCMAKE_CXX_FLAGS=-Wall -Werror"
cmake --build build