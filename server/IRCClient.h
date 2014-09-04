#include <string>
#include <list>

#include "CharBuffer.h"

class IRCClient
{
	private:
		CharBuffer myBuffer;
		int mySocket;
		std::string channel;

	public:
		IRCClient();
		~IRCClient();

		void close();
		bool isGood();

		void putLine( std::string msg );
		std::string getLine();

		int connectChannel(
				std::string serverIP,
				int port,
				std::string channel,
				std::string nickName
		);
};
