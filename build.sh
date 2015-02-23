#!/bin/bash -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cd "${DIR}/src/library/index"
protoc --cpp_out .  index.proto
protoc --cpp_out .  dictionary.proto

cd "${DIR}/src/tools/index_parser"
./build.sh release

cd "${DIR}/src/tools/dictionary_parser"
./build.sh release

cd "${DIR}/src/tools/ranker"
./build.sh release
