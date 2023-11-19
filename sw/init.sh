rm -rf build/
mkdir build
cd build/
cmake ..
make -j 32 mnist_spike
