#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <string>
#include <list>

#include "TCPSocketServer.h"
#include "WebSocketConnection.h"
#include "IRCClient.h"
#include "Configurator.h"

class ChatServer : public TCPSocketServer
{
	std::list<WebSocketConnection*> participants;
	IRCClient ircConnection;
	std::list<std::string> msgLog;
	Configurator config;

	public:
	ChatServer();
	bool connect();
	void ircLoop();
	void listeningLoop();

	private:
	void addLogLine( std::string msg );
	void participantLoop( int connection_fd );
	void notifyIRC( std::string name, std::string msg );
	void notifyParticipants( std::string name, std::string msg );
	void notifyParticipants( std::string msg );
	void notifyParticipants( std::string name, unsigned int status );
};

#endif // CHAT_SERVER_H
