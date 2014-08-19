#include "TCPSocketConnection.h"

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

TCPSocketConnection::TCPSocketConnection(int connection_fd)
	: clientSocket_fd( connection_fd )
{
}

TCPSocketConnection::~TCPSocketConnection()
{
	close();
}

bool TCPSocketConnection::open()
{
	return true;
}

bool TCPSocketConnection::readLine()
{
	std::string message = myBuff.getLine();

	while( message.compare("") == 0 )
	{
		fetchData();
		message = myBuff.getLine();
	}

	for( auto it = listeners.begin(); it != listeners.end(); it++ )
	{
		(*it)->notify( message );
	}

	return true;
}

void TCPSocketConnection::close()
{
	std::cout << "Disconnecting client" << std::endl;
	::close(clientSocket_fd);
	clientSocket_fd = -1;
}

bool TCPSocketConnection::good()
{
	return clientSocket_fd >= 0;
}

void TCPSocketConnection::fetchData()
{
	if( !good() ) return;

	char cBuffer[1024];
	int n;
//	do {
		n = ::read( clientSocket_fd, cBuffer, 1024 );
		if( n < 0 )
		{
			std::cerr << "ERROR reading from socket\n";
			close();
		}
		else if( n == 0 )
		{
			std::cout << "Received eof, closing" << std::endl;
			close();
		}
		else
		{
			myBuff.append( cBuffer, n );
		}
//	} while (n == 1024);
	// do/while -> questionable approach
	// if msg length is really 1024, this would cause another
	// blocking call to read. if then there is no additional
	// incomming msg -> blocked for long time
}

std::string TCPSocketConnection::getLine()
{
	if( !good() )
	{
//		std::cerr << "ERROR no valid connection for reading!\n";
		return "";
	}

	std::string returnString = myBuff.getLine();

	if( returnString.compare("") == 0 )
	{
		fetchData();
		return myBuff.getLine();
	}

	return returnString;
}

void TCPSocketConnection::putLine( char* s, unsigned int length )
{
	if( !good() ) return;

	int n = ::write( clientSocket_fd, s, length );
	if( n < 0 )
	{
		std::cerr << "ERROR writing to socket\n";
		close();
	}
	else if( n < length )
	{
		std::cerr << "ERROR not all bytes sent\n";
	}
}

void TCPSocketConnection::putLine( std::string s )
{
	if( !good() ) return;

	int n = ::write( clientSocket_fd, s.c_str(), s.length() );
	if( n < 0 )
	{
		std::cerr << "ERROR writing to socket\n";
		close();
	}
	else if( n < s.length() )
	{
		std::cerr << "ERROR not all bytes sent\n";
	}
}

void TCPSocketConnection::addListener( Listener* listener )
{
	for( auto it = listeners.begin(); it != listeners.end(); it++ )
	{
		if( (*it) == listener ) return;
	}
	listeners.push_back( listener );
}

void TCPSocketConnection::removeListener( Listener* listener )
{
	listeners.remove( listener );
}
