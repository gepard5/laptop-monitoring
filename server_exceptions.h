/*
 * =====================================================================================
 *
 *       Filename:  server_exceptions.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10.05.2017 19:16:24
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <exception>
#include <stdexcept>

class ServerDisconnected : public std::logic_error {
	public:
		ServerDisconnected() : std::logic_error ( "Server disconnected") {}
};

class UnexpectedToken : public std::logic_error {
	public:
		UnexpectedToken() : std::logic_error ( "Unexpected token") {}
};

class ServerClosingSignal : public std::logic_error {
	public:
		ServerClosingSignal() : std::logic_error ( "Server closing signal") {}
};
