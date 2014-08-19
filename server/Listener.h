#ifndef LISTENER_H
#define LISTENER_H

#include <string>

class Listener
{
	public:
		virtual void notify( std::string msg );
};

#endif // LISTENER_H
