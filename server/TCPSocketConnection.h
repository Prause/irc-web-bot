#ifndef TCP_SOCKET_CONNECTION_H
#define TCP_SOCKET_CONNECTION_H

#include <string>
#include "CharBuffer.h"
#include "Listener.h"

class TCPSocketConnection {
	private:
		int clientSocket_fd; // socket file desc. for client connection
	protected:
		CharBuffer myBuff;
		std::list<Listener*> listeners;

	public:
		TCPSocketConnection(int connection_fd);
		~TCPSocketConnection();

		virtual bool open();
		bool readLine();
		virtual void close();

		bool good();

		void addListener( Listener* listener );
		void removeListener( Listener* listener );

		std::string getLine();

		void putLine( char* s, unsigned int length );

		void putLine( std::string s );

	protected:
		void fetchData();

};

#endif // TCP_SOCKET_CONNECTION_H
