
clear; clc;

% Compile the test for Mex Arrays
mex CXXFLAGS="\$CXXFLAGS -std=c++0x -Wall -O2" test_mex_arrays.cpp

% Create test matrix
A = toeplitz(1:32); 
A = A(1:7,1:19);

    disp('Input the following matrix:'); A
    B = test_mex_arrays(A);
    disp('The output array is:'); B