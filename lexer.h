/*
 * =====================================================================================
 *
 *       Filename:  lexer.h
 *
 *    Description:  Parsing icinga configuration and showing connections between its objects
 *
 *        Version:  1.0
 *        Created:  17.04.2017 22:51:
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Michał Glinka
 *   Organization:  Politechnika Warszawska
 *
 * =====================================================================================
 */
#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>
#include <string>
#include <vector>
#include <set>

#include "token.h"
#include "source.h"


class Lexer {
	public:
		Lexer();

		Token getNextToken(Source& source);

	private:
		typedef std::set<std::string> StringSet;
		Token getMessageEdge( Source& );

		Token::TYPE getTokenType( const std::string& ) const;
		Token::TYPE getTokenType( char ) const;

		bool isObjectEdge( const std::string& s ) const
		{ return object_edges.count(s) == 1; }

		bool isSeparator( char c ) const
		{ return separators.count(c) == 1; }

		bool isKeyValueSeparator( char c ) const
		{ return key_value_separators.count(c) == 1; }

		bool isWhitespace( char c ) const
		{ return whitespace.count(c) == 1; }

		bool isStringEnd( char c ) const
		{ return string_end.count(c) == 1; }

		bool isSingleCharToken(char c) const
		{ return single_char_tokens.count(c) == 1; }

		StringSet object_edges;
		std::set<char> separators;
		std::set<char> key_value_separators;
		std::set<char> single_char_tokens;
		std::set<char> whitespace;
		std::set<char> string_end;
};

#endif
