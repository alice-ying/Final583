patricia: patricia.c patricia_test.c
	clang -emit-llvm -c patricia.c -o patricia.bc
	clang -emit-llvm -c patricia_test.c -o patricia_test.bc
	llvm-link patricia.bc patricia_test.bc -o patricia_agg.bc

clean:
	rm -rf patricia output*
