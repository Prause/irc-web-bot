#ifndef CHAR_BUFFER_H
#define CHAR_BUFFER_H

#include <list>
#include <string>
#include <thread>
#include <mutex>

class CharBuffer { // TODO: change to template
	private:
		std::mutex rwLock;
		std::list<char*> activeBuffers; // TODO: change to use vectors
		unsigned int readPos;
		unsigned int writePos;
		const unsigned int chunkSize;

	public:
		CharBuffer(unsigned int chunkSize = 1024);

		void diagnostic() const;

		bool contains( char character );

		void append( char* buffer, unsigned int count );

		char peekAt(unsigned int pos) const;

		unsigned int getCount() const;

		std::string getLine();

		unsigned int getBlock( char* buffer, unsigned int length );

		std::string getWebsocketMsg();
};

#endif // CHAR_BUFFER_H
