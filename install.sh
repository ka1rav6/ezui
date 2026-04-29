#!/usr/bin/env bash

set -e

echo "Installing EzUI..."

# Install dependencies
sudo apt update
sudo apt install -y libsdl2-dev libsdl2-ttf-dev cmake g++

# Clone repo
git clone https://github.com/YOUR_USERNAME/ezui.git
cd ezui

# Build
mkdir -p build && cd build
cmake ..
cmake --build .

# Install headers + lib
sudo cmake --install .

echo "EzUI installed!"