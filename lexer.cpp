/*
 * =====================================================================================
 *
 *       Filename:  lexer.cpp
 *
 *    Description:  Parsing icinga configuration and showing connections between its objects
 *
 *        Version:  1.0
 *        Created:  17.04.2017 22:51:59
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Michał Glinka
 *   Organization:  Politechnika Warszawska
 *
 * =====================================================================================
 */

#include "lexer.h"
#include "server_exceptions.h"


Lexer::Lexer() {
	object_edges = { "#####" };
	separators = { ',' };
	key_value_separators = { ':' };

	single_char_tokens = { ',', ':' };
	whitespace = { ' ', '\t', '\n' };
	string_end = { ' ', '\t', '\n', ',', ':', '#'};
}

Token Lexer::getNextToken(Source& source)
{
	//skip white characters
	while( isWhitespace( source.peekChar() ) ) {
		source.getChar();
	}

	char curr =  source.getChar();
	if( isSingleCharToken( curr ) ) {
		std::string curr_token = std::string(1, curr);
		auto type = getTokenType( curr );
		return Token( curr_token, type );
	}

	if( curr == '#' ) {
		return getMessageEdge( source );
	}

	std::string curr_token = std::string(1, curr);
	while( !isStringEnd( source.peekChar() ) ) {
		curr_token.push_back( source.getChar() );
	}

	return Token( curr_token, getTokenType( curr_token ) );
}

Token Lexer::getMessageEdge( Source& source ) {
	std::string buf = "#";
	while( source.peekChar() == '#' ) {
		buf.push_back( source.getChar() );
		if( isObjectEdge( buf ) ) {
			return Token( buf, getTokenType( buf ) );
		}
	}

	throw UnexpectedToken();
}

Token::TYPE Lexer::getTokenType( char c ) const
{
	if( isSeparator( c ) ) 			return Token::SEPARATOR;
	if( isKeyValueSeparator( c ) ) 	return Token::KEY_VALUE_SEPARATOR;
	return Token::UNRECOGNISED;
}

Token::TYPE Lexer::getTokenType( const std::string& token ) const
{
	if( isObjectEdge(token) )		return Token::OBJECT_EDGE;
	return Token::STRING;
}

