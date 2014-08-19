#include <iostream>

#include "ChatServer.h"

int main(int argc, char *argv[])
{

	if (argc < 2) {
		std::cerr << "ERROR, no port provided\n";
		return 1;
	}

	ChatServer server( std::stoi(argv[1]) );
	if( server.connect() )
	{
		server.listeningLoop();
	}
	else
	{
		std::cerr << "Error: Could not start ChatServer.\n";
	}

	return 0; 
}
