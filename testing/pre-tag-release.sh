#! /bin/bash

set -e
root=$(git rev-parse --show-toplevel)

cd $root

bindir="$PWD/build-and-test"
function cleanup()
{
  rm -r $bindir
}
trap cleanup EXIT

function copy_bindir()
{
  cp -r $bindir $bindir.error
}
trap copy_bindir ERR

echo "Checking that project can be installed."
mkdir $bindir
cd $bindir
conan install ${root} -g cmake_paths
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_paths.cmake -DCMAKE_INSTALL_PREFIX=$bindir/install
cmake --build .
cmake --build . --target test

# test install
cmake --build . --target install





echo "Checking that installed project can be detected and used."
mkdir app
cd app

cat << EOF > main.cpp
#include <iostream>
#include <libArrhenius/version.h>

int main()
{
  std::cout << "VERSION: " << libArrhenius_VERSION << std::endl;
  std::cout << "FULL VERSION: " << libArrhenius_VERSION_FULL << std::endl;

  return 0;
}
EOF

cat << EOF > CMakeLists.txt
cmake_minimum_required(VERSION 3.1)
add_executable( main main.cpp )
find_package( libArrhenius REQUIRED )
target_link_libraries(main libArrhenius::Arrhenius )
set_target_properties(main PROPERTIES CXX_STANDARD 11)
EOF

mkdir build2
cd build2
conan install ${root} -g cmake_paths
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_paths.cmake -DCMAKE_INSTALL_PREFIX=$bindir/install
cmake --build .
./main

cd ..







echo "PASSED"

exit 0
