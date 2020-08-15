server.out client.out:server.cpp client.cpp
	g++ server.cpp -o server.out -pthread
	g++ client.cpp -o client.out -pthread
	
.PHONY:clean
	rm -rf *.o