# Sketch of compiling an example (polyroots.c here)
clang -I ../../install/include/  -emit-llvm -c -O0  polyroots.c 
/data/klee/klee-afr/Release+Asserts/bin/klee  --link-llvm-lib=../../install/lib/libgsl.bca  --link-llvm-lib=../../install/lib/libgslcblas.bca --libc=uclibc  --link-llvm-lib=/data/klee/klee-afr/Release+Asserts/lib/klee-uclibc-libm.bca polyroots.bc
