#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <thread>

#include "IRCClient.h"

void readLoop( IRCClient* client )
{
	while( client->isGood() )
	{
		std::cout << client->getLine() << std::endl;
	}
}

int main( int argc, char* argv[])
{
	IRCClient* myClient = new IRCClient();

	myClient->connectChannel("erik2");

	std::thread t1 ( readLoop, myClient );

	std::string buffer = "blub";
	while( buffer.compare( "quit" ) != 0 ) {
		std::getline( std::cin, buffer );
		if( buffer.compare( "quit" ) == 0 ) {
			myClient->close();
			break;
		}

		myClient->putLine( buffer );
	}

	t1.join();

	return 0;
}
