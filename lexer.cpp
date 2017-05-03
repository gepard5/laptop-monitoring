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
	//source empty, nothing to read
	if( source.peekChar() == EOF ) {
		return Token();
	}

	if( !buf.empty() ) {
		if( buf.front() == '#' ) {
			return getMessageEdge( source );
		}

		return getStringMessage( source );
	}


	//skip white characters
	while( source.peekChar() != EOF && isWhitespace( source.peekChar() ) ) {
		source.getChar();
	}

	if( source.peekChar() == EOF ) {
		return Token();
	}

	char curr =  source.getChar();
	if( isSingleCharToken( curr ) ) {
		std::string curr_token = std::string(1, curr);
		auto type = getTokenType( curr );
		return Token( curr_token, type );
	}

	if( curr == '#' ) {
		buf = std::string( 1, curr );
		return getMessageEdge( source );
	}

	std::string curr_token = std::string(1, curr);
	while( source.peekChar() != EOF && !isStringEnd( source.peekChar() ) ) {
		curr_token.push_back( source.getChar() );
	}

	if( source.peekChar() == EOF ) {
		buf = curr_token;
		return Token();
	}

	return Token( curr_token, getTokenType( curr_token ) );
}

Token Lexer::getMessageEdge( Source& source ) {
	while( source.peekChar() == '#' ) {
		buf.push_back( source.getChar() );
		if( isObjectEdge( buf ) ) {
			std::string curr_token = buf;
			buf = std::string();
			return Token( curr_token, getTokenType( curr_token ) );
		}
	}

	if( source.peekChar() != EOF ) {
		throw UnexpectedToken();
	}

	return Token();
}

Token Lexer::getStringMessage( Source& source ) {
	while( source.peekChar() != EOF && !isStringEnd( source.peekChar() ) ) {
		buf.push_back( source.getChar() );
	}

	if( source.peekChar() == EOF ) {
		return Token();
	}
	std::string curr_token = buf;
	buf = std::string();
	return Token( curr_token, getTokenType( curr_token ) );
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

