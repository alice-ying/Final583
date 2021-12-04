FILE1 = basicmath_small.c   rad2deg.c  cubic.c   isqrt.c
FILE2 = basicmath_large.c   rad2deg.c  cubic.c   isqrt.c

all: basicmath_small basicmath_large

basicmath_small: ${FILE1} Makefile
	clang -emit-llvm -include snipmath.h -c basicmath_small.c -o basicmath_small.bc
	clang -emit-llvm -include snipmath.h -c cubic.c -o cubic.bc
	clang -emit-llvm -include snipmath.h -c isqrt.c -o isqrt.bc
	clang -emit-llvm -include snipmath.h -c rad2deg.c -o rad2deg.bc
	llvm-link basicmath_small.bc cubic.bc isqrt.bc rad2deg.bc -o agg.bc
basicmath_large: ${FILE2} Makefile


clean:
	rm -rf basicmath_small basicmath_large output*
