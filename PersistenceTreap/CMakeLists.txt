cmake_minimum_required(VERSION 3.15)
project(Treap)

set(CMAKE_CXX_STANDARD 17)

add_executable(Treap main.cpp treap.h treap.cpp PersistentTreapHeap.h)