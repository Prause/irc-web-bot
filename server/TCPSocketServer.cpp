#include "TCPSocketServer.h"

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

TCPSocketServer::TCPSocketServer()
	: portNo(0), socket_fd(-1)
{
}

TCPSocketServer::~TCPSocketServer()
{
	std::cout << "Closing TCPSocketServer." << std::endl;
	close();
}

bool TCPSocketServer::connect( unsigned int portNo )
{
	this->portNo = portNo;

	// open socket
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0)
	{
		std::cerr << "ERROR opening socket\n";
		return false;
	}

	// force reconnect if port in use
	int doReconnect=1;
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR,
			&doReconnect, sizeof(doReconnect));

	// bind to port
	struct sockaddr_in serv_addr;
	std::memset(&serv_addr, 0, sizeof(serv_addr));
//		bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(this->portNo);
	int status = bind(
			socket_fd,
			(struct sockaddr *) &serv_addr,
			sizeof(serv_addr));
	if( status < 0)
	{
		  std::cerr << "ERROR on binding" << std::endl;
		  close();
		  return false;
	}
	return true;
}

int TCPSocketServer::listenForIncoming()
{
	// listen for incoming connection
	listen(socket_fd,1);

	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientSocket_fd = accept(socket_fd, 
			(struct sockaddr *) &clientAddr, 
			&clientAddrLen);
	if( clientSocket_fd < 0 )
	{
		std::cerr << "ERROR on accept\n";
	}
	return clientSocket_fd;
}

void TCPSocketServer::close()
{
	std::cout << "Closing port." << std::endl;
	::close(socket_fd);
	socket_fd = -1;
}

bool TCPSocketServer::good()
{
	return socket_fd >= 0;
}

