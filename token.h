/*
 * =====================================================================================
 *
 *       Filename:  token.h
 *
 *    Description:  Parsing icinga configuration and showing connections between its objects
 *
 *        Version:  1.0
 *        Created:  17.04.2017 22:50:44
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author: Michał Glinka
 *   Organization: Politechnika Warszawska
 *
 * =====================================================================================
 */

#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>
#include <string>

#include <map>

class Token {
	public:
		enum TYPE {
			OBJECT_EDGE,								//koniec definicji obiektu: "}"
			KEY_VALUE_SEPARATOR,
			SEPARATOR,
			STRING,									//ciągi znaków
			UNRECOGNISED,
			END_OF_FILE							//koniec pliku
		};


		Token() : type(TYPE::UNRECOGNISED) {}
		Token(const std::string& v, TYPE t ) :
		 type(t), value(v)	{}

		void printInfo() const;

		TYPE getType() const
		{ return type; }

		std::string getValue() const
		{ return value; }

	private:
		TYPE type;
		std::string value;
};

#endif

