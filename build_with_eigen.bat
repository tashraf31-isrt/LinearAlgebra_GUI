# Download Eigen (header-only) if not present
if not exist Eigen3 (
    curl -L -o eigen.zip https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.zip
    tar -xf eigen.zip
    move eigen-3.4.0 Eigen3
    del eigen.zip
)

g++ -I Eigen3 -std=c++17 -o main.exe new.cpp Matrix.cpp
