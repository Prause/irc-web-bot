#include <iostream>
#include <cstring> // strlen()
#include <sys/socket.h> // socket()
//#include <sys/wait.h> // waitpid()
#include <signal.h>
#include <arpa/inet.h> // inet_addr()

int main( int argc, char* argv[])
{
    int mySocket = socket(AF_INET, SOCK_STREAM, 0);
	if( mySocket == -1 )
	{
		std::cerr << "Socket couldn't be initialized!" << std::endl;
		return 1;
	}

    struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(6667);
	serv_addr.sin_addr.s_addr = inet_addr("172.20.65.121");

	if( connect( mySocket, (sockaddr*) &serv_addr, sizeof(serv_addr) ) == -1 )
	{
		std::cerr << "Could not connect to server!" << std::endl;
		return 1;
	}

	pid_t pid = fork();
	if( pid != 0 )
	{
		std::string buffer;
		std::cout << "Type 'quit' to exit. "
			<< "All data will be sent to "
			<< "172.20.65.121:6667..."
			<< std::endl;
		buffer = "NICK erik\r\n";
		write( mySocket, buffer.c_str(), buffer.size() );
		buffer = "USER erik erik_host erik_server :erik\r\n";
		write( mySocket, buffer.c_str(), buffer.size() );
		sleep( 3 );
		buffer = "JOIN #m6\r\n";
//		buffer = "PRIVMSG rhodium blub\r\n";
		write( mySocket, buffer.c_str(), buffer.size() );
		while( buffer.compare( "quit" ) != 0 ) {
			std::getline( std::cin, buffer );
			if( buffer.compare( "quit" ) == 0 ) break;

			buffer = "PRIVMSG #m6 :" + buffer + "\r\n";
			if( write( mySocket, buffer.c_str(), buffer.size() ) == -1 )
			{
				std::cerr << "Could not send message!" << std::endl;
				close( mySocket );
				return 1;
			}
		}
	}
	else
	{
		while ( true ) {
			sleep( 1 );
			char cBuffer[4096];
			int n = 4096;
			while( n == 4096 ) {
				n = read( mySocket, cBuffer, 4096 );
				if( n == -1 )
				{
					std::cerr << "Failed reading from socket!" << std::endl;
					close( mySocket );
					return 1;
				}
				std::string sBuffer ( cBuffer, n );
				std::cout << sBuffer;
				
				if( sBuffer.compare( 0, 4, "PING" ) == 0 )
				{
					if( write( mySocket, "PONG\r\n", 6 ) == -1 )
					{
						std::cerr << "Could not send message!" << std::endl;
					}
				}

// reference: :Dante!dante@bouncer.hadiko.de PRIVMSG #m6 :blub
				std::size_t found = sBuffer.find(" PRIVMSG #m6 :");
				if( found != std::string::npos )
				{
					std::string message = sBuffer.substr( found + 14 );
					found = sBuffer.find("!");
					std::string sender = sBuffer.substr( 1, found - 1 );
					std::cout << sender << ": " << message << std::endl;
				}
			}
		}
	}

//	int status;
//	waitpid( pid, &status, 0 );

	kill(pid, SIGTERM);
	close( mySocket );

	return 0;
}
