# Cellular Noise

## Initialize Submodules
```
git submodule update --init --recursive
```

## Ubuntu/Debian Dependencies
```
sudo apt-get install libwayland-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev pkg-config libxkbcommon-dev
```

## Build and Run
```
# build
mkdir build
cd build
cmake ..
make -j

# run
./CellularNoise
```
