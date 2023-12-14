cc=g++
cflags=-c -Wall
gcc=$(cc) $(cflags)
log1=log_server_1
log2=log_server_2

all: servers client

servers: server_1.o server_2.o $(log1).o $(log2).o

client: client.o

client.o: src/client.cpp
	$(gcc) src/client.cpp -o bin/client

server_1.o: src/server_1.cpp
	$(gcc) src/server_1.cpp -o bin/server_1

$(log1).o: src/$(log1).cpp
	$(gcc) src/$(log1).cpp -o bin/$(log1)

server_2.o: src/server_1.cpp
	$(gcc) src/server_2.cpp -o bin/server_2

$(log2).o: src/$(log2).cpp
	$(gcc) src/$(log2).cpp -o bin/$(log2)

clean:
	rm -f bin/*.o all
