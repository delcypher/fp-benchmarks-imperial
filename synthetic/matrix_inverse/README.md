To compile:

gcc -o main inverse.c -DFLOAT_TYPE=<FLOAT_TYPE> -DN=<N> -std=c99

e.g.:

gcc -o main inverse.c -DFLOAT_TYPE=float -DN=3 -std=c99
