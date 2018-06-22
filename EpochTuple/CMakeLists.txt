cmake_minimum_required(VERSION 3.10)
project(EpochTuple)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE
    Epoch.h
    EpochTuple.h
    EpochTupleView.h
)

add_executable(test_epoch_tuple ${SOURCE} test/test_epoch_tuple.cpp)
add_executable(samples_triangle ${SOURCE} samples/triangle.cpp)
add_executable(samples_pyramid  ${SOURCE} samples/pyramid.cpp)
add_executable(samples_settings ${SOURCE} samples/settings.cpp)