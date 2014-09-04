#include <iostream>

#include "ChatServer.h"

int main(int argc, char *argv[])
{

	ChatServer server;
	if( server.connect() )
	{
		server.listeningLoop();
	}
	else
	{
		std::cerr << "Error: Could not start ChatServer.\n";
		return 1;
	}

	return 0; 
}
