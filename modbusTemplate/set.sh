mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-arm-none-eabi.cmake ..
make
