FILE1 = basicmath_small.c   rad2deg.c  cubic.c   isqrt.c
FILE2 = basicmath_large.c   rad2deg.c  cubic.c   isqrt.c

all: basicmath_small basicmath_large

basicmath_small: ${FILE1} Makefile
	clang -emit-llvm -c basicmath_small.c -o basicmath_small.bc
	clang -emit-llvm -c cubic.c -o cubic.bc
	clang -emit-llvm -c isqrt.c -o isqrt.bc
	clang -emit-llvm -c rad2deg.c -o rad2deg.bc
	clang -emit-llvm -c pi.h -o pi.bc
	clang -emit-llvm -c round.h -o round.bc
	clang -emit-llvm -c snipmath.h -o snipmath.bc
	clang -emit-llvm -c sniptype.h -o sniptype.bc
	llvm-link basicmath_small.bc cubic.bc isqrt.bc rad2deg.bc pi.bc round.bc snipmath.bc sniptype.bc -o agg.bc
	llvm-link basicmath_small.bc cubic.bc isqrt.bc rad2deg.bc -o agg.bc
basicmath_large: ${FILE2} Makefile


clean:
	rm -rf basicmath_small basicmath_large output*
