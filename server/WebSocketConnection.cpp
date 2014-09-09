#include "WebSocketConnection.h"

#include <iostream>
#include <cstring>

#include <openssl/sha.h>
#include "base64.cpp"


const std::string WebSocketConnection::wsKey_GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

WebSocketConnection::WebSocketConnection( int connection_fd )
	: TCPSocketConnection( connection_fd )
{
	sessionKey = "";
}

bool WebSocketConnection::open()
{
	if( !sessionKey.empty() )
	{
		std::cerr << "Error: WebSocket already open.\n";
		return false;
	}
	TCPSocketConnection::open();

	std::string receivedLine = getLine();
	while( TCPSocketConnection::good()
			&& !receivedLine.empty()
			&& receivedLine.compare("\r") != 0 )
	{
		if( receivedLine.compare(0,19,"Sec-WebSocket-Key: ") == 0 )
		{
			
			// TODO this part is errror prone
			std::string key = receivedLine.substr( receivedLine.find(":") + 2 );
			key = key.substr( 0, key.length() - 1 );
std::cout << "Incoming key: " << key << std::endl;
			std::string combinedKey = key + wsKey_GUID;
			unsigned char outBuff[20];
			SHA1( (const unsigned char*) combinedKey.c_str(),
					combinedKey.length(),
					outBuff );
			sessionKey = base64_encode( outBuff, 20);
		}
		receivedLine = getLine();
	}

	if( sessionKey.empty() )
	{
		std::cerr << "Error: Could not open WebSocket." << std::endl;
		return false;
	}

	TCPSocketConnection::putLine("HTTP/1.1 101 Switching Protocols\r\n");
	TCPSocketConnection::putLine("Upgrade: websocket\r\n");
	TCPSocketConnection::putLine("Connection: Upgrade\r\n");
	std::string sendItem = "Sec-WebSocket-Accept: " + sessionKey + "\r\n";
std::cout << "Session key: " << sessionKey << std::endl;
	TCPSocketConnection::putLine(sendItem);
	TCPSocketConnection::putLine("\r\n");
	return true;
}

void WebSocketConnection::close()
{
	if( isGood() )
	{
		// TODO send proper websocket-close msg
	}
	std::cout << "Ended WS connection." << std::endl;
	sessionKey = "";
	TCPSocketConnection::close();
}

bool WebSocketConnection::isGood()
{
	return (!sessionKey.empty()) && TCPSocketConnection::good();
}

std::string WebSocketConnection::getWebsocketMsg()
{
	if( !isGood() ) return "";

	TCPSocketConnection::fetchData();
//	if( myBuff.getCount() != 0 )
//		std::cout << std::hex << (int) myBuff.peekAt(0) << std::dec << " / " << myBuff.getCount() << std::endl;
	if( myBuff.getCount() < 2 ) return "";
	if( myBuff.peekAt( 0 ) & 0x08 )
	{
		close();
		return "";
	}
	if( (myBuff.peekAt( 0 ) & 0x80) == 0 )
	{
		std::cout << "fragmented" << std::endl;
		putLine( "Message was to long to be received." );
		return ""; // TODO at least remove package from queue
	}

	unsigned int msgLength = myBuff.peekAt( 1 ) & 0x7F;
	unsigned int maskLength = 0;
	unsigned int lengthOffset = 0;
	unsigned int headerLength = 0;
	if( myBuff.peekAt( 1 ) & 0x80 ) maskLength = 4;

	if( msgLength == 126 )
	{
		lengthOffset = 2;
		if( myBuff.getCount() < 2 + lengthOffset + maskLength ) return "";
		msgLength = myBuff.peekAt(2);
		msgLength <<= 8;
		unsigned int tmp = myBuff.peekAt(3);
		msgLength |= tmp;
	}

	if( myBuff.getCount() < 2 + lengthOffset + maskLength + msgLength || msgLength == 127 )
	{
		std::cout << "over-length" << std::endl;
		putLine( "Message was to long to be received." );
		return ""; // TODO at least remove package from queue
	}
	
	char* memBlock = new char[2 + lengthOffset + maskLength + msgLength];
	myBuff.getBlock( memBlock, 2 + lengthOffset + maskLength + msgLength );

	if( maskLength > 0 )
	{
		for( int i = 0; i < msgLength; i++ )
		{
			memBlock[2+maskLength+i+lengthOffset] = memBlock[2+maskLength+i+lengthOffset] ^ memBlock[2+lengthOffset+(i%maskLength)];
		}
	}
	std::string returnString (&memBlock[2 + maskLength + lengthOffset], msgLength);

	delete memBlock;
	return returnString;
}

void WebSocketConnection::putLine(std::string line)
{
	if( !isGood() ) return;

	if(line.size() >= 126)
	{
		line = line.substr( 0, 125 - 6 );
		line += " [...]";
	}

	if(line.size() < 126)
	{
		char* frame = new char[ line.size() + 2 ];
		char lenByte = (char) line.size();
		std::memcpy( &frame[2], line.c_str(), line.size() );
		frame[0] = 0x81; // not fragmented, text payload
		frame[1] = lenByte;
//			frame[line.size()+1] = 0;
//			std::cout << frame << std::endl;

		TCPSocketConnection::putLine( frame, line.size() + 2 );

		delete frame;
	}
}
