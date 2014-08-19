#include "WebSocketServer.h"

#include <iostream>
#include <string>
#include <cstring>

#include <openssl/sha.h>

#include "base64.cpp"



const std::string WebSocketServer::wsKey_GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

WebSocketServer::WebSocketServer( unsigned int portNo )
	: TCPSocketServer(portNo)
{
	sessionKey = "";
}

bool WebSocketServer::connect()
{
	if( TCPSocketServer::connect() && TCPSocketServer::listenForIncoming() )
	{
		std::string receivedLine = getLine();
		while( good() && !receivedLine.empty() && receivedLine.compare("\r") != 0 )
		{
			if( receivedLine.compare(0,19,"Sec-WebSocket-Key: ") == 0 )
			{
				// TODO this part is errror prone
				std::string key = receivedLine.substr( receivedLine.find(":") + 2 );
				key = key.substr( 0, key.length() - 1 );
				std::string combinedKey = key + wsKey_GUID;
				unsigned char outBuff[20];
				SHA1( (const unsigned char*) combinedKey.c_str(),
						combinedKey.length(),
						outBuff );
				sessionKey = base64_encode( outBuff, 20);
			}
			receivedLine = getLine();
		}
		TCPSocketServer::putLine("HTTP/1.1 101 Switching Protocols\r\n");
		TCPSocketServer::putLine("Upgrade: websocket\r\n");
		TCPSocketServer::putLine("Connection: Upgrade\r\n");
		std::string sendItem = "Sec-WebSocket-Accept: ";
		sendItem += sessionKey;
		sendItem += "\r\n";
		TCPSocketServer::putLine(sendItem);
		TCPSocketServer::putLine("\r\n");
		return true;
	}

	return false;
}

bool WebSocketServer::isConnected()
{
	return TCPSocketServer::good() && !sessionKey.empty();
}

std::string WebSocketServer::getWebsocketMsg()
{
	if( !isConnected() ) return "";

	TCPSocketServer::fetchData();
	if( myBuff.getCount() < 2 ) return "";
	if( myBuff.peekAt( 0 ) & 0x08 )
	{
		std::cout << "end" << std::endl;
		sessionKey = ""; // invalidates this WebSocket connection
		return "";
	}
	if( (myBuff.peekAt( 0 ) & 0x80) == 0 )
	{
		std::cout << "fragmented" << std::endl;
		return "";
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
		return "";
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

void WebSocketServer::putLine(std::string line)
{
	if( !isConnected() ) return;

	if(line.size() < 126)
	{
		char* frame = new char[ line.size() + 2 ];
		char lenByte = (char) line.size();
		std::memcpy( &frame[2], line.c_str(), line.size() );
		frame[0] = 0x81; // not fragmented, text payload
		frame[1] = lenByte;
//			frame[line.size()+1] = 0;
//			std::cout << frame << std::endl;

		TCPSocketServer::putLine( frame, line.size() + 2 );

		delete frame;
	}
}
