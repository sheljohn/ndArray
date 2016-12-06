
clear; clc;

% Compile the test for ndArrays
mex -largeArrayDims CXXFLAGS="\$CXXFLAGS -std=c++0x -Wall -O2" test_ndArray.cpp

% Create test matrix
A = toeplitz(1:32);
A = A(1:7,1:19);

    disp('Input the following matrix:'); A
    B = test_ndArray(A);
    disp('The output array is:'); B
