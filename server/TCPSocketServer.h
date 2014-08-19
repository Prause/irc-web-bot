#ifndef TCP_SOCKET_SERVER_H
#define TCP_SOCKET_SERVER_H

#include <string>

class TCPSocketServer {
	private:
		unsigned int portNo;
		int socket_fd; // socket file descriptor (for listening for incoming connections)

	public:
		TCPSocketServer(unsigned int portNo);
		~TCPSocketServer();

		bool connect();
		int listenForIncoming();

		void close();
		bool good();

};

#endif // TCP_SOCKET_SERVER_H
