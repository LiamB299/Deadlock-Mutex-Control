mutex: mutex.o Server.o
	g++ -std=c++11 -o mutex mutex.o Server.o -lpthread 

mutex.o: mutex.cpp Server.h
	g++ -c mutex.cpp

Server.o: Server.cpp Server.h
	g++ -c Server.cpp

run:
	./mutex batman.ascii 6

clean:
	rm *.o mutex
