#include <iostream>
#include <queue>
#include <mutex>
#include <unistd.h>
#include <thread>
#include "Server.h"
#include <fstream>
#include <string>

using namespace std;

struct Message
{
	string payload;
	unsigned int t_ind;
};

class MsgPass
{
public:
	MsgPass(unsigned int upper_b)
	{
		up_bound = upper_b;
	}

	~MsgPass(){}

	void send(struct Message m)
	{
		msgqueue.push(m);
	}

	struct Message recv()
	{
		struct Message msg = msgqueue.front();
		msgqueue.pop();
		
		return msg;
	}

	bool isFull()
	{
		bool toReturn = false;

		if (msgqueue.size() == up_bound)
			toReturn = true;

		return toReturn;
	}

	unsigned int getSize()
	{
		return msgqueue.size();
	}

	bool empty()
	{
		return msgqueue.empty();
	}

private:
	queue<Message> msgqueue;
	unsigned int up_bound;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void manage(int);
void lock(int);
void unlock(int);
void passAlong(int);
void getAndWrite();
void print(string);
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Server *server;
MsgPass *m;
mutex sout;
mutex serv;
mutex lprint;
mutex lmsg;
mutex out;
int spin=0;

void manage(int id) {

	while (spin!=id) {}; //schedule

	while(spin==id) {
		try {
			passAlong(id);
		}
		catch (exception *e) {
			unlock(0);
			print("Thread " + to_string(id) +  ": Lock released for Server\n");
			delete e;
		}
	}
}

void print(string toPrint) { 
	lock(3);
	cout<<toPrint;	
	unlock(3);
}

void output(string out) { //reading to textfile
	lock(1); 
	ofstream outfile;
	cout<<"Opening..."<<endl;
	outfile.open("mutex.txt", ios::out | ios::app);
	cout<<"Writing..."<<endl;
	outfile<<out;
	outfile.close();	
	unlock(1);
}

void lock(int choice) {
	if (choice==0)
		serv.try_lock();
	else if (choice==1)
		out.try_lock();
	else if (choice==2)
		lmsg.try_lock();
	else if (choice==3)
		lprint.try_lock();	
}

void unlock(int choice) {
	if (choice==0)
		serv.unlock();
	else if (choice==1)
		out.unlock();
	else if (choice==2)
		lmsg.unlock();
	else if (choice==3)
		lprint.unlock();		
}

void getAndWrite()
{
	uint size = m->getSize();
	string sline="", swrite; 
	Message temp[size];
	for (int i=0; i<size; i++) {
		temp[i] = m->recv();
		output("Receiving Thread "+ to_string(i)+"'s message from message queue\n");
	}
	//output(sline);

	//output(temp[size-1].payload); //+"\n");
	for (int i=size-1; i>-1; i--) { 
		output(temp[i].payload);
	}
}

void passAlong(int index)
{
	try
	{
		if (m->isFull())
		{
			thread mediator = thread(getAndWrite);
		}
		else
		{
			lock(0); // server
			print("Thread " + to_string(index) +  ": Lock acquired for Server\n");
			string piece = server->getPiece(index);
			unlock(0);

			print("Thread " + to_string(index) +  ": Lock released for Server\n");

			Message msg;
			//cout<<piece;
			msg.payload = piece;
			//cout<<endl<<index<<endl;
			msg.t_ind = index;
			//cout<<endl<<msg.t_ind<<endl;

			lock(2); //class lock
			m->send(msg);
			unlock(2);

			/* locked in function */
			output("Thread " + to_string(index) + " sending message\n");

			spin++; //schedule threads
		}
	}
	catch (const char* err)
	{
		cout << err << endl;
		throw new exception();
	}
}

int main(int argc, char const *argv[]) //treaat main as mediator?
{
	string sfilename, hold;
	int threads;
	cout<<"Number of threads and filename: ";
	cin>>threads;
	cin>>sfilename;

	if (sfilename != "" && threads != 0)
	{
		
		ofstream clearfile;
		clearfile.open("mutex.txt", ostream::out | ios_base::trunc);
		clearfile.close();

		thread th[threads];
		server = new Server(sfilename, threads);
		m = new MsgPass(threads);

		for (int i=0; i<threads; i++) {
			th[i] = thread(manage, i);
		}

		for (int j=0; j<threads; j++) {
			th[j].join();
		}

		print("Thread 0: Lock acquired for Printer\n");
		thread mediator = thread(getAndWrite);
		mediator.join();
		print("Thread 0: Lock released for Printer\n");

		delete server;
		delete m;
	}
	else
	{
		cout << "Please enter a file name and the number of threads to use!" << endl;
	}
	return 0;
}