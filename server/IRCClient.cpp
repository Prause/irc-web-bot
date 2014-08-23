#include <iostream>
#include <cstring> // strlen()
#include <sys/socket.h> // socket()
//#include <signal.h>
#include <arpa/inet.h> // inet_addr()
#include <unistd.h>


#include "IRCClient.h"

IRCClient::IRCClient()
{
	mySocket = -1;
}

IRCClient::~IRCClient()
{
	close();
}

void IRCClient::close()
{
	::close( mySocket );
	mySocket = -1;
}

bool IRCClient::isGood()
{
	return mySocket >= 0;
}

void IRCClient::putLine( std::string msg )
{
	if( !isGood() ) return;
	msg = msg + "\r\n";
	msg = "PRIVMSG #m6 :" + msg + "\r\n";

	if( write( mySocket, msg.c_str(), msg.size() ) == -1 )
	{
		std::cerr << "Could not send message!" << std::endl;
//		close( mySocket );
//		return 1;
	}
}

std::string IRCClient::getLine()
{
	if( !isGood() ) return "";
//	else std::cout << "reading" << std::endl;

	int n = 0;
	char cBuffer[1024];

	while( ! myBuffer.contains( '\n' ) ) // TODO sensitivity for eof -> close connection
	{
//		std::cout << "read..." << std::endl;
		n = read( mySocket, cBuffer, 1024 );
//		std::cout << "@@@ " << cBuffer << std::endl;
		myBuffer.append( cBuffer, n );
//		std::cout << "...done" << std::endl;
	}

	std::string retVal = myBuffer.getLine();
	while( retVal.size() > 0 )
	{
//		std::cout << "### " << retVal;
		if( retVal.compare( 0, 4, "PING" ) == 0 )
		{
			if( write( mySocket, "PONG\r\n", 6 ) == -1 )
			{
				std::cerr << "Could not send PONG!" << std::endl;
			}
		}

		std::size_t found = retVal.find(" PRIVMSG #m6 :");
		if( found != std::string::npos )
		{
			std::string message = retVal.substr( found + 14 );
			found = retVal.find("!");
			std::string sender = retVal.substr( 1, found - 1 );
			return (sender + ": " + message);
		}

		retVal = myBuffer.getLine();
	}
	return "";
}

int IRCClient::connectChannel( std::string name )
{
	mySocket = socket(AF_INET, SOCK_STREAM, 0);
	if( mySocket == -1 )
	{
		std::cerr << "Socket couldn't be initialized!" << std::endl;
		return false;
	}

	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(6667);
	serv_addr.sin_addr.s_addr = inet_addr("193.196.36.22");

	if( connect( mySocket, (sockaddr*) &serv_addr, sizeof(serv_addr) ) == -1 )
	{
		std::cerr << "Could not connect to server!" << std::endl;
		/*
		   close(mySocket);
		   mySocket = -1;
	   */
		return false;
	}

	std::cout << "### Starting IRC registration." << std::endl;
	std::string buffer;

	std::cout << "### Setting nick name." << std::endl;
	buffer = "NICK " + name + "\r\n";
	write( mySocket, buffer.c_str(), buffer.size() );

	// Check if setting nick was successful
	// searching for " NOTICE Auth :Welcome"
	// negative: "Nickname is already in use"

	std::cout << "### Setting user name." << std::endl;
	buffer = "USER " + name;
	buffer += " " + name + "_hst";
	buffer += " " + name + "_srv";
	buffer += " :" + name + "\r\n";
	write( mySocket, buffer.c_str(), buffer.size() );

	sleep( 3 );
	std::cout << "### Joining channel." << std::endl;
	buffer = "JOIN #m6\r\n";
	write( mySocket, buffer.c_str(), buffer.size() );

	std::cout << "### Connected." << std::endl;

	return true;
}
