/*
 * =====================================================================================
 *
 *       Filename:  stringSource.cpp
 *
 *    Description:  Parsing icinga configuration and showing connections between its objects
 *
 *        Version:  1.0
 *        Created:  19.04.2017 22:01:50
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Michał Glinka
 *   Organization:  Politechnika Warszawska
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "bufferSource.h"

char BufferSource::getChar() {
	if( msg_buffer.empty() ) {
		return EOF;
	}

	char c = msg_buffer.front();
	msg_buffer.pop_front();
	return c;
}


char BufferSource::peekChar() {
	return msg_buffer.empty() ? EOF : msg_buffer.front() ;
}

void BufferSource::addToBuffer( const char* buf, int len ) {
	for( int i = 0; i < len ; ++i ) 
	{
		msg_buffer.push_back( *buf );
		++buf;
	}
}
