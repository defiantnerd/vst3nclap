cmake_minimum_required(VERSION 3.22)

# Setzen Sie den Projektnamen und die verwendete Sprache
project(vst3nclaptest CXX)

# Setzen Sie den C++-Standard auf C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Fügen Sie die ausführbare Datei mit der Quelldatei hinzu
add_executable(vst3nclaptest src/main.cpp)

