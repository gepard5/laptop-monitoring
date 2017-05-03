/*
 * =====================================================================================
 *
 *       Filename:  source.h
 *
 *    Description:  Parsing icinga configuration and showing connections between its objects
 *
 *        Version:  1.0
 *        Created:  19.04.2017 20:55:07
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Michał Glinka
 *   Organization:  Politechnika Warszawska
 *
 * =====================================================================================
 */
#ifndef SOURCE_H
#define SOURCE_H


class Source {
	public:
		virtual char getChar() = 0;
		virtual char peekChar() = 0;
		virtual int getRow() const = 0;
		virtual int getColumn() const = 0;
};

#endif
