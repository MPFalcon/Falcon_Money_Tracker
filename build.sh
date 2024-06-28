rm -rf ./build
./clang_script.sh

mkdir build
cd build
cmake ..
make