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

using namespace std;

//=============================================================

Server *server;
int sp=0;
mutex printer;
mutex serv;
mutex out;		//Std out protection
void lock(int);
void unlock(int);
void manage(int);

//=============================================================

void printToScreen(string toPrint) {
	out.lock();
	cout<<toPrint;	
	out.unlock();
	/* Implement this function so that printing from each thread to stdout (i.e. using cout) doesn't clash with each other. */
}
//============================================================

void print(string out) {
	ofstream outfile;
	cout<<"Opening..."<<endl;
	outfile.open("deadlock.txt", ios_base::app);
	cout<<"Writing..."<<endl;
	outfile<<out;
	outfile.close();
}
//===========================================================
void lock(int choice) {
	if (choice==0) {
		serv.lock();
	}
	else if (choice==1) {
		printer.lock();
	}
}

void unlock(int choice) 
{
	if (choice==0) {
		serv.unlock();
	}
	else if (choice==1) {
		printer.unlock();
	}	
}

void spin(int index) {

	while (index!=sp) {}

	lock(1); //printer
	printToScreen("Thread " + to_string(index) +  ": Lock acquired for Printer\n");	
}

void evenThread(int index)
{
	try
	{
		spin(index);

		lock(0); // server
		printToScreen("Thread " + to_string(index) +  ": Lock acquired for Server\n");
		string piece = server->getPiece(index);

		print(piece);
		
		unlock(0);
		printToScreen("Thread " + to_string(index) +  ": Lock released for Server\n");
		
		unlock(1); // printer
		printToScreen("Thread " + to_string(index) +  ": Lock released for Printer\n");
	}
	catch (const char *msg)
	{
		cerr << msg << endl;
	}
}

void oddThread(int index)
{
	try
	{
		lock(0); 
		printToScreen("Thread " + to_string(index) +  ": Lock acquired for Server\n");
		string piece = server->getPiece(index);

		spin(index);
		print(piece);
		
		unlock(0); 
		printToScreen("Thread " + to_string(index) +  ": Lock released for Server\n");
		
		unlock(1); // printer
		printToScreen("Thread " + to_string(index) +  ": Lock released for Printer\n");
	}
	catch (const char *msg)
	{
		cout << msg << endl;
	}
}

void manage(int index) {
	
	if ((index % 2)>0) {
			oddThread(index); }
		else {
			evenThread(index); }
	sp++;	
}

int main(int argc, char const *argv[]) //arg 1 is filename; arg 2 is the number of threads
{
	int threads = atoi(argv[2]);

	if (argv[1] != "" && threads != 0)
	{
		ofstream fclear;
		fclear.open("deadlock.txt", ostream::out | ios_base::trunc);
		fclear.close();

		server = new Server(argv[1], threads);
		thread th[threads];

		for (int i=0; i<threads; i++) {
			th[i] = thread(manage, i);
			int ran = rand()/static_cast<float>(RAND_MAX);
			sleep(ran);	
		}

		//cout<<"Creation complete\n";

		for (int k=0; k<threads; k++) {
			th[k].join();
		}
		delete server;
	}
	else
	{
		cout << "Please enter a file name and the number of threads to use!" << endl;
	}

	return 0;
}

//===================================================================