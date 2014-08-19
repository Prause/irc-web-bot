#ifndef WEB_SOCKET_SERVER_H
#define WEB_SOCKET_SERVER_H

#include <string>

#include "TCPSocketServer.h"

class WebSocketServer : public TCPSocketServer
{
	static const std::string wsKey_GUID;
	std::string sessionKey;

	public:
	WebSocketServer( unsigned int portNo );

	bool connect();

	std::string getWebsocketMsg();

	void putLine(std::string line);

	bool isConnected();
};

#endif // WEB_SOCKET_SERVER_H
