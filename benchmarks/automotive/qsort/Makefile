FILE1 = qsort_small.c
FILE2 = qsort_large.c

all: qsort_small qsort_large

qsort_small: ${FILE1} Makefile
	clang -emit-llvm -c qsort_small.c -o qsort_small.bc

qsort_large: ${FILE2} qsort_large.c Makefile
	clang -emit-llvm -c qsort_large.c -o qsort_large.bc

clean:
	rm -rf qsort_small qsort_large output*