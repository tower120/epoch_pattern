cmake_minimum_required(VERSION 3.10)
project(EpochTuple)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE
    Epoch.h
    EpochTuple.h
    EpochTupleView.h
)

add_executable(test_epoch_tuple ${SOURCE} test/test_epoch_tuple.cpp)