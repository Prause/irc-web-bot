#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Datastructures
#include <boost/test/unit_test.hpp>

#include "CharBuffer.h"
 
BOOST_AUTO_TEST_CASE(sizeTests)
{
	CharBuffer buffer;
	BOOST_CHECK_EQUAL( buffer.getCount(), 0 );

	buffer.append( "foobar", 6 );
	BOOST_CHECK_EQUAL( buffer.getCount(), 6 );

	buffer.append( "foobar", 5 );
	BOOST_CHECK_EQUAL( buffer.getCount(), 11 );

	std::string out = buffer.getLine();
	BOOST_CHECK_EQUAL( buffer.getCount(), 11 );
	BOOST_CHECK_EQUAL( out.length(), 0 );

	buffer.append( "blub\nbla", 8 );
	BOOST_CHECK_EQUAL( buffer.getCount(), 19 );

	out = buffer.getLine();
	BOOST_CHECK_EQUAL( buffer.getCount(), 3 );
	BOOST_CHECK_EQUAL( out.length(), 15 ); // outputline is without \n

	CharBuffer buffer2( 32 );
	BOOST_CHECK_EQUAL( buffer2.getCount(), 0 );
	
	int count = 0;
	for( int i = 0; i < 107; i++ )
	{
		buffer2.append( "la\nblab", 7 );
		count += 7;
		BOOST_CHECK_EQUAL( buffer2.getCount(), count );
	}

	out = buffer2.getLine();
	count -= 3;
	BOOST_CHECK_EQUAL( buffer2.getCount(), count );
	BOOST_CHECK_EQUAL( out.length(), 2 );

	for( int i = 0; i < 106; i++ )
	{
		out = buffer2.getLine();
		count -= 7;
		BOOST_CHECK_EQUAL( buffer2.getCount(), count );
		BOOST_CHECK_EQUAL( out.length(), 6 );
	}

	for( int i = 0; i < 109; i++ )
	{
		buffer2.append( "\r\t\n\a", 4 );
		count += 4;
		BOOST_CHECK_EQUAL( buffer2.getCount(), count );
	}

	char tmpBuff[8];
	for( int i = 0; i < 25; i++ )
	{
		buffer2.getBlock( tmpBuff, 8 );
		count -= 8;
		BOOST_CHECK_EQUAL( buffer2.getCount(), count );
	}

	for( int i = 0; i < 48; i++ )
	{
		buffer2.getBlock( tmpBuff, 5 );
		count -= 5;
		BOOST_CHECK_EQUAL( buffer2.getCount(), count );
	}

	BOOST_CHECK_EQUAL( buffer2.getCount(), 0 );
	buffer2.getBlock( tmpBuff, 5 );
	BOOST_CHECK_EQUAL( buffer2.getCount(), 0 );

}

