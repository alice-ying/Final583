all:  dijkstra_large dijkstra_small

dijkstra_large: dijkstra_large.c Makefile
	clang -emit-llvm -c dijkstra_large.c -o dijkstra_large.bc

dijkstra_small: dijkstra_small.c Makefile
	clang -emit-llvm -c dijkstra_small.c -o dijkstra_small.bc

clean:
	rm -rf dijkstra_large dijkstra_small output*
