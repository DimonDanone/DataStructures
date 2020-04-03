SET(CMAKE_CXX_FLAGS -pthread)
set(CMAKE_CXX_STANDARD 17)

add_library(rpForest rpForest.cpp rpTree.h pointForRpTree.h rpForest.h rpTreeNode.h)