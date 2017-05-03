/*
 * =====================================================================================
 *
 *       Filename:  stringSource.h
 *
 *    Description:  Parsing icinga configuration and showing connections between its objects
 *
 *        Version:  1.0
 *        Created:  19.04.2017 21:58:59
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Michał Glinka
 *   Organization:  Politechnika Warszawska
 *
 * =====================================================================================
 */

#ifndef STRING_SOURCE_H
#define STRING_SOURCE_H

#include <stdlib.h>
#include <string>
#include <deque>
#include "source.h"

class BufferSource : public Source {
	public:
		BufferSource() {}
		~BufferSource() {}
		char getChar();
		char peekChar();
		int getRow() const
		{ return 0; }

		int getColumn() const 
		{ return 0; }

		void addToBuffer( const char* buf, int len );

	private:
		std::deque<char> msg_buffer;
};

#endif
