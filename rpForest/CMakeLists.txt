cmake_minimum_required(VERSION 3.15)
project(rpForest)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS -pthread)

add_executable(rpForest main.cpp rpTree.h rpTree.cpp pointForRpTree.h rpForest.h rpTreeNode.h log_duration.h)