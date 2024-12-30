BINARY=./build/rmus
LIBMA=./build/libma

all: rmus.c
	gcc rmus.c -o $(BINARY) -lm $(LIBMA).a
	$(BINARY)
	rm $(BINARY)

install: libma all

libma:
	gcc -c miniaudio.c -o $(LIBMA).o
	ar rcs $(LIBMA).a $(LIBMA).o
	rm $(LIBMA).o
