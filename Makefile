BINARY=./build/rmus
LIBMA=./build/libma

rmus: rmus.c
	gcc rmus.c -o $(BINARY) -lm $(LIBMA).a

install: libma all

libma:
	gcc -c miniaudio.c -o $(LIBMA).o
	ar rcs $(LIBMA).a $(LIBMA).o
	rm $(LIBMA).o

uninstall:
	rm $(BINARY)
	rm $(LIBMA)

run: rmus
	$(BINARY)
