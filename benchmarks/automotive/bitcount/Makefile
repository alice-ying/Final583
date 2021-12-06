FILE = bitcnt_1.c bitcnt_2.c bitcnt_3.c bitcnt_4.c bitcnts.c bitfiles.c bitstrng.c bstr_i.c 

bitcnts: ${FILE} Makefile
	clang -emit-llvm -c bitcnt_1.c -o bitcnt_1.bc
	clang -emit-llvm -c bitcnt_2.c -o bitcnt_2.bc
	clang -emit-llvm -c bitcnt_3.c -o bitcnt_3.bc
	clang -emit-llvm -c bitcnt_4.c -o bitcnt_4.bc
	clang -emit-llvm -c bitcnts.c -o bitcnts.bc
	clang -emit-llvm -c bitfiles.c -o bitfiles.bc
	clang -emit-llvm -c bitstrng.c -o bitstrng.bc
	clang -emit-llvm -c bstr_i.c -o bstr_i.bc
	llvm-link bitcnt_1.bc bitcnt_2.bc bitcnt_3.bc bitcnt_4.bc bitcnts.bc bitfiles.bc bitstrng.bc bstr_i.bc -o bitcnts_agg.bc
	
clean:
	rm -rf bitcnts output*