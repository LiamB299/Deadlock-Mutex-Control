deadlock: deadlock.o Server.o
	g++ -o deadlock deadlock.o Server.o -lpthread 

deadlock.o: deadlock.cpp Server.h
	g++ -c deadlock.cpp

Server.o: Server.cpp Server.h
	g++ -c Server.cpp

run:
	./deadlock batman.ascii 3

clean:
	rm *.o deadlock
