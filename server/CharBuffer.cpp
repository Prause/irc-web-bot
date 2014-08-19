#include "CharBuffer.h"

#include <iostream>
#include <cstring>
#include <algorithm>

//#define DEBUG true

CharBuffer::CharBuffer(unsigned int chunkSize)
	: readPos(0), writePos(0), chunkSize(chunkSize)
{}

void CharBuffer::diagnostic() const
{
	std::cout << "Buffers: " << activeBuffers.size()
		<< " (" << getCount() << " chars)"
		<< ", r/w: " << readPos << "/" << writePos << std::endl;
}

void CharBuffer::append( char* buffer, unsigned int count )
{
#if DEBUG
	std::cout << "append() ";
	diagnostic();
#endif

	if( count == 0 ) return;

	std::lock_guard<std::mutex> guard(rwLock);

	if( activeBuffers.empty() ) activeBuffers.push_back( new char[ chunkSize ] );

	unsigned int copied = 0;
	while( count - copied > 0 )
	{
		if( count - copied > chunkSize - writePos )
		{
			std::memcpy(
					&(activeBuffers.back()[writePos]),
					&buffer[copied],
					chunkSize - writePos );
			copied += chunkSize - writePos;
			writePos = 0;
			activeBuffers.push_back( new char[ chunkSize ] );
		}
		else
		{
			std::memcpy(
					&(activeBuffers.back()[writePos]),
					&buffer[copied],
					count - copied );
			writePos += count - copied;
			copied = count;
		}
	}

#if DEBUG
	std::cout << "\t";
	diagnostic();
#endif
}

bool CharBuffer::contains(char character)
{
#if DEBUG
	std::cout << "contains() ";
	diagnostic();
#endif

	std::lock_guard<std::mutex> guard(rwLock);

	auto foundIter = activeBuffers.begin();
	for( ; foundIter != activeBuffers.end(); foundIter++ )
	{
		const char* start = *foundIter;
		if(*foundIter == activeBuffers.front()) start = *foundIter + readPos;
		const char* end = *foundIter + chunkSize;
		if(*foundIter == activeBuffers.back()) end = *foundIter + writePos;
		const char* endline = std::find( start, end, character ); // TODO perhaps also search for \0, \r ?
		if( endline != end ) // FOUND!
		{
			return true;
		}
	}
	return false;
}

std::string CharBuffer::getLine()
{
#if DEBUG
	std::cout << "getLine() ";
	diagnostic();
#endif

	std::lock_guard<std::mutex> guard(rwLock);

	auto foundIter = activeBuffers.begin();
	for( ; foundIter != activeBuffers.end(); foundIter++ )
	{
		const char* start = *foundIter;
		if(*foundIter == activeBuffers.front()) start = *foundIter + readPos;
		const char* end = *foundIter + chunkSize;
		if(*foundIter == activeBuffers.back()) end = *foundIter + writePos;
		const char* endline = std::find( start, end, '\n' ); // TODO perhaps also search for \0, \r ?
		if( endline != end ) // FOUND!
		{
			std::string returnString = "";
			// TODO returnString.reserve(chunkSize*(blub+1));
			for( auto it = activeBuffers.begin(); it != foundIter; )
			{
//								returnString += std::string( *it + readPos, it* + chunkSize );
				returnString += std::string( *it + readPos, chunkSize - readPos );
				readPos = 0;
				delete[] *it;
				it++;
				activeBuffers.pop_front();
			}
			// read last chunk:
//							returnString += std::string( *foundIter + readPos, (const char*) endline );
			returnString += std::string( *foundIter + readPos, endline - *foundIter - readPos );
			readPos = endline + 1 - *foundIter;
			if( *foundIter == activeBuffers.back() && readPos == writePos )
			{
				readPos = 0;
				writePos = 0;
			}
			if( readPos == chunkSize )
			{
				readPos = 0;
				delete[] *foundIter;
				activeBuffers.pop_front();
			}

#if DEBUG
			std::cout << "\t";
			diagnostic();
#endif
			return returnString;
		}
	}

#if DEBUG
	std::cout << "\t";
	diagnostic();
#endif
	return "";
}

char CharBuffer::peekAt(unsigned int pos) const
{
	if( pos >= getCount() ) return 0;

	auto it = activeBuffers.begin();
	while( readPos + pos >= chunkSize )
	{
		it++;
		pos -= chunkSize;
	}
	return (*it)[ pos + readPos ];
}

unsigned int CharBuffer::getBlock( char* buffer, unsigned int length )
{
#if DEBUG
	std::cout << "getBlock() ";
	diagnostic();
#endif
	if( length > getCount() ) return 0;

	std::lock_guard<std::mutex> guard(rwLock);

	unsigned int copied = 0;
	while( length > copied )
	{
		if( readPos + length - copied < chunkSize )
		{
			// remainder fits into current chunk + extra space
			std::memcpy(
					&buffer[copied],
					&(activeBuffers.front()[readPos]),
					length - copied );
			readPos += length - copied;
			copied = length;

			if( activeBuffers.size() == 1 && readPos == writePos )
			{
				readPos = 0;
				writePos = 0;
			}
		}

		else
		{
			// remainder fills current chunk completely
			std::memcpy(
					&buffer[copied],
					&(activeBuffers.front()[readPos]),
					chunkSize - readPos );
			copied += chunkSize - readPos;
			readPos = 0;
			delete[] activeBuffers.front();
			activeBuffers.pop_front();
			if( activeBuffers.empty() ) writePos = 0; // writePos has been chunkSize
		}
	}

#if DEBUG
	std::cout << "\t";
	diagnostic();
#endif
	return 0;
}

/*
std::string CharBuffer::getWebsocketMsg()
{
#if DEBUG
	std::cout << "getWebsocketMsg() ";
	diagnostic();
#endif

	if( activeBuffers.empty() ) return "";
	if( peekAt( 0 ) & 0x08 )
	{
		std::cout << "end" << std::endl;
		return "";
	}
	if( peekAt( 0 ) & 0x80 == 0 )
	{
		std::cout << "fragmented" << std::endl;
		return "";
	}

	unsigned int msgLength = peekAt( 1 ) & 0x7F;
	unsigned int maskLength = 0;
	unsigned int lengthOffset = 0;
	if( peekAt( 1 ) & 0x80 ) maskLength = 4;
	
#if DEBUG
	std::cout << "msgLength: " << msgLength << std::endl;
#endif

	if( msgLength == 126 )
	{
		msgLength = peekAt(2);
		msgLength <<= 8;
		unsigned int tmp = peekAt(3);
		msgLength |= tmp;
		lengthOffset = 2;
#if DEBUG
		std::cout << "msgLength: " << msgLength << std::endl;
#endif
	}

	if( getCount() < msgLength + 2 + maskLength || msgLength == 127 )
	{
		return "";
	}
	
	char* memBlock = new char[msgLength + 2 + maskLength + lengthOffset];
	getBlock( memBlock, msgLength + 2 + maskLength + lengthOffset );
//	memBlock[msgLength + 2 + maskLength + lengthOffset - 1] = 0;
//	std::cout << memBlock; 
	if( maskLength > 0 )
	{
		for( int i = 0; i < msgLength; i++ )
		{
			memBlock[2+maskLength+i+lengthOffset] = memBlock[2+maskLength+i+lengthOffset] ^ memBlock[2+lengthOffset+(i%maskLength)];
		}
	}
	std::string returnString (&memBlock[2 + maskLength + lengthOffset], msgLength);

#if DEBUG
	std::cout << "\t";
	diagnostic();
#endif

	delete memBlock;
	return returnString;
}
*/

unsigned int CharBuffer::getCount() const
{
	if( activeBuffers.size() > 1 )
	{
		return (chunkSize - readPos) + chunkSize*(activeBuffers.size() - 2) + writePos;
	}
	return writePos - readPos;
}

