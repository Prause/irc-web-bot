SOURCES=websocket_server.cpp CharBuffer.cpp TCPSocketServer.cpp TCPSocketConnection.cpp WebSocketConnection.cpp ChatServer.cpp Listener.cpp IRCClient.cpp
all:
	g++ -std=c++0x -pthread -o wsServer -Wall $(SOURCES) -lcrypto

CharBuffer.o:
	g++ -c -std=c++0x -o CharBuffer.o -Wall CharBuffer.cpp CharBuffer.h

test:
	g++ -std=c++0x CharBuffer.cpp CharBufferTests.cpp -lboost_unit_test_framework
	./a.out
