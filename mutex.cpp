#include <iostream>
#include <fstream>
#include <unistd.h>
#include "Server.h"
#include <sstream>
#include <pthread.h>
#include <iomanip>
#include <mutex>
#include <thread>
#include <cstdlib>
#include <exception>

using namespace std;

//=============================================================

Server *server;
int sp=0;
bool blocked=false; 
mutex printer;
mutex serv;
mutex sout;

void evenThread(int);
void oddThread(int);
void lock(int);
void unlock(int);

//=============================================================

void printToScreen(string toPrint) {
	sout.try_lock();
	cout<<toPrint;	
	sout.unlock();
}

void print(string out){
	sout.try_lock();
	ofstream outfile;
	cout<<"Opening..."<<endl;
	outfile.open("mutex.txt", ios_base::app);
	cout<<"Writing..."<<endl;
	outfile<<out;
	outfile.close();
	sout.unlock();
}

void lock(int choice) {
	if (choice==1) {
		serv.lock();
	}
	else {
		printer.lock();
	}
}

void unlock(int choice) {
	if (choice==1) {
		serv.unlock();
	}
	else {
		printer.unlock();
	}

}

void spin(int index) {
	while (index!=sp) {
	}
}

void manage(int id) {
	while (id==sp) {	
		try {	
			if ((id % 2)>0) {
				oddThread(id);
			}
			else
				evenThread(id);
		} //try

		catch (const exception* e) {

				cout<<"Caught and continuing... "<<endl<<endl;

				unlock(1); //unlock server
				printToScreen("Thread " + to_string(sp) +  ": Lock released for Server\n");
				}		
	}	
}
void evenThread(int index) {
	//try {
	try {
		spin(index);
			
		printToScreen("Thread " + to_string(index) +  ": Lock acquired for Server\n");

		lock(1); // server
		string piece = server->getPiece(index); 
		unlock(1); //server

		printToScreen("Thread " + to_string(index) +  ": Lock released for Server\n");	

		printToScreen("Thread " + to_string(index) +  ": Lock acquired for Printer\n");	

		lock(0);	//printer	
		print(piece);	
		unlock(0);
		
		printToScreen("Thread " + to_string(index) +  ": Lock released for Printer\n");
		sp++; //added
	}
	catch (const char *msg) {
		cerr << msg << endl;
		throw new exception();
	}
}

void oddThread(int index) {
	//try {
	try {
		spin(index);

		printToScreen("Thread " + to_string(index) +  ": Lock acquired for Server\n");

		lock(1); // server	
		string piece = server->getPiece(index);
		unlock(1);

		lock(0);
		print(piece);
		unlock(0);

		printToScreen("Thread " + to_string(index) +  ": Lock released for Server\n");
		
		printToScreen("Thread " + to_string(index) +  ": Lock released for Printer\n");

		sp++; //added
	}
	catch (const char *msg)
	{
		cout << msg << endl;
		throw new exception();
	}	
}

int main(int argc, char const *argv[]) {
	string sfilename, hold;
	int threads, ran;
	/* cin here */

	ifstream clearfile;
	clearfile.open("mutex.txt", ofstream::out | ios_base::trunc);
	clearfile.close();

	cout<<"Number of threads and filename:";
	cin>>threads;
	cin>>sfilename;

	if (sfilename != "" && threads != 0)
	{
		server = new Server(sfilename, threads);
		
		thread th[threads];
		for (int i=0; i<threads; i++) {
			th[i] = thread(manage, i);
			th[i].join();
		}		
		delete server;
		}	
	else
	{
		cout << "Please enter a file name and the number of threads to use!" << endl;
	}		

	return 0;

}//main
