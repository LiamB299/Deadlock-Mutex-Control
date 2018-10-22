#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <vector>
#include <fstream>
#include <mutex>

using namespace std;

class Server
{
public:
	Server();
	Server(string, int);
	~Server();
	string getPiece(int);
private:
	string *ascii;
	mutex access;
};

#endif