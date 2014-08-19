#ifndef WEB_SOCKET_CONNECTION_H
#define WEB_SOCKET_CONNECTION_H

#include <string>

#include "TCPSocketConnection.h"

class WebSocketConnection : public TCPSocketConnection
{
	static const std::string wsKey_GUID;
	std::string sessionKey;

	public:
	WebSocketConnection( int connection_fd );
	bool open();
	bool isGood();
	void close();

	std::string getWebsocketMsg();
	void putLine( std::string line );
};

#endif // WEB_SOCKET_CONNECTION_H
