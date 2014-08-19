#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <string>
#include <list>

#include "TCPSocketServer.h"
#include "WebSocketConnection.h"
#include "IRCClient.h"

class ChatServer : public TCPSocketServer
{
	std::list<WebSocketConnection*> participants;
	IRCClient ircConnection;

	public:
	ChatServer( unsigned int portNo );
	bool connect();
	void ircLoop();
	void listeningLoop();

	private:
	void participantLoop( int connection_fd );
	void notifyIRC( std::string name, std::string msg );
	void notifyParticipants( std::string name, std::string msg );
	void notifyParticipants( std::string msg );
	void notifyParticipants( std::string name, unsigned int status );
};

#endif // CHAT_SERVER_H
