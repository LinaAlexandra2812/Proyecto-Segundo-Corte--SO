all: rversions rversionsd

rversions: client.o protocol.o sha256.o versions.o
	gcc -o rversions client.o protocol.o sha256.o versions.o

rversionsd: server.o protocol.o sha256.o versions.o
	gcc -o rversionsd server.o protocol.o sha256.o versions.o

%.o: %.c
	gcc -c $< -o $@

clean:
	rm -r *.o rversions rversionsd
	rm -rf docs

clean-repo:
	rm -rf .versions

doc:
	doxygen

