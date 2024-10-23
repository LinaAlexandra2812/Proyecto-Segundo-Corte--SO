all: client.o server.o protocol.o
	gcc -o server server.o protocol.o
	gcc -o client client.o protocol.o

%.o: %.c
	gcc -c $< -o $@

clean:
	rm -r *.o client server docs

doc:
	doxygen