#include "Server.h"
#include <unistd.h>
#include <time.h>

Server::Server(){}

Server::Server(string filename, int threads)
{
	vector<string> loaded;
	ascii = new string[threads];
	ifstream in;
	string line;
	in.open(filename);
	if (!in.is_open())
	{
		cout << "Could not open file " << filename << endl;
		exit(1);
	}
	while(!in.eof())
	{
		getline(in, line);
		loaded.push_back(line);
	}
	in.close();

	int step = loaded.size()/threads;
	string piece = "";

	for (int i = 0; i < threads; ++i)
	{
		for (int j = step*i; j < ((step*i) + step); ++j)
		{
			if (j + 1 < loaded.size())
				piece += loaded.at(j) + "\n";
			else
				piece += loaded.at(j);
		}
		ascii[i] = piece;
		piece = "";
	}
}

Server::~Server()
{
	delete []ascii;
}

string Server::getPiece(int piece)
{
	srand(time(NULL));
	if (rand()/static_cast<float>(RAND_MAX) > 0.80)
		throw "500 Internal Server Error";
	cout << "Getting piece number " << piece << endl;
	string toReturn = ascii[piece];
	return toReturn;
}