all:
	gcc -o programaTrab *.c -g

run:
	./programaTrab

debug:
	valgrind ./programaTrab -v
	# -v -s --track-origins=yes --leak-check=full

zip:
	zip trab3 *.c *.h Makefile