
CC=gcc

randpass: Makefile randpass.c
	$(CC) randpass.c -o randpass

clean:
	rm -f randpass Makefile~ randpass.c~ randpass~ a.out
