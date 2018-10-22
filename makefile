msgpass: msgpass.o Server.o
	g++ -o msgpass msgpass.o Server.o -lpthread

msgpass.o: msgpass.cpp Server.h
	g++ -c msgpass.cpp

Server.o: Server.cpp Server.h
	g++ -c Server.cpp

run:
	./msgpass
	
clean:
	rm *.o msgpass
