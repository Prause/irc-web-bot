#include "ChatServer.h"

#include <iostream>
#include <thread>

ChatServer::ChatServer( unsigned int portNo )
	: TCPSocketServer( portNo )
{}

bool ChatServer::connect()
{
	if( TCPSocketServer::connect() )
	{
		if( ircConnection.connectChannel("Robot") ) return true;
		else close();
	}
	return false;
}

void ChatServer::ircLoop()
{
	while( ircConnection.isGood() )
	{
		std::string msg = ircConnection.getLine();
		if( ! msg.empty() )
		{
			notifyParticipants( msg );
		}
	}
}

void ChatServer::listeningLoop()
{
	std::cout << "started loops" << std::endl;

	std::thread t0 ( &ChatServer::ircLoop, this );
	t0.detach();

	while( good() )
	{
		int connection_fd = listenForIncoming();
		std::cout << "incoming" << std::endl;
		std::thread t1 (
				&ChatServer::participantLoop, this, connection_fd );
		t1.detach();
		std::cout << "detached" << std::endl;
		// TODO thread pool
	}
	close();
}

void ChatServer::participantLoop( int connection_fd )
{
	std::cout << "participant loop" << std::endl;

	WebSocketConnection newConnection (connection_fd);
	if( newConnection.open() )
	{
		std::string partName = "";
		while( newConnection.isGood() && partName.empty() )
		{
			// query name as long as no name is provided
			partName = newConnection.getWebsocketMsg();
		}
		if( newConnection.isGood() ) // => partName is not empty
		{
			participants.push_back(&newConnection);
			notifyParticipants( partName, 1 );
			while( newConnection.isGood() ) {
				std::string incoming = newConnection.getWebsocketMsg();
				if( !incoming.empty() ) // only accept non-empty messages
				{
					notifyParticipants( partName, incoming );
					notifyIRC( partName, incoming );
				}
				incoming = "";
			}
			participants.remove(&newConnection);
			// TODO: scope-bound destruction of pointer using subscriber-pattern (-> stack-unwinding)
			notifyParticipants( partName, 0 );
		}
	}
}

void ChatServer::notifyParticipants( std::string name, unsigned int status )
{
	time_t rawtime;
	time( &rawtime);
	struct tm* timeinfo = localtime(&rawtime);
	char buff[9];
	strftime( buff, 9, "(%R) ", timeinfo);

	std::string statusMsg = status == 1
		? " joined conversation."
		: " left conversation.";
	// TODO mutexify
	for( auto it = participants.begin(); it != participants.end(); it++ )
	{
		(*it)->putLine( buff + name + statusMsg );
	}
	std::cout << buff << name << statusMsg << std::endl;
}

void ChatServer::notifyParticipants( std::string msg )
{
	time_t rawtime;
	time( &rawtime);
	struct tm* timeinfo = localtime(&rawtime);
	char buff[9];
	strftime( buff, 9, "(%R) ", timeinfo);

	// TODO mutexify
	for( auto it = participants.begin(); it != participants.end(); it++ )
	{
		(*it)->putLine( buff + msg );
	}
	std::cout << buff << msg << std::endl;
}

void ChatServer::notifyParticipants( std::string name, std::string msg )
{
	time_t rawtime;
	time( &rawtime);
	struct tm* timeinfo = localtime(&rawtime);
	char buff[9];
	strftime( buff, 9, "(%R) ", timeinfo);

	// TODO mutexify
	for( auto it = participants.begin(); it != participants.end(); it++ )
	{
		(*it)->putLine( buff + name + ": " + msg );
	}
	std::cout << buff << name << ": " << msg << std::endl;
}

void ChatServer::notifyIRC( std::string name, std::string msg )
{
	if( !ircConnection.isGood() ) return;
	ircConnection.putLine( name + ": " + msg );
}
