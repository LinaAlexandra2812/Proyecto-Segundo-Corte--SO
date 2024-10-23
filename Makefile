all: client.o server.o protocol.o sha256.o versions.o
	gcc -o server server.o protocol.o sha256.o versions.o
	gcc -o client client.o protocol.o sha256.o versions.o

%.o: %.c
	gcc -c $< -o $@

clean:
	rm -r *.o client server docs

doc:
	doxygen
