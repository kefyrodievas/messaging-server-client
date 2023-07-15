client:
	gcc -o client src/client.c -lpthread

server:
	gcc -o server src/server.c -lpthread

clean:
	rm server client

runc: client
	./client

runs: server
	./server