/*
 * =====================================================================================
 *
 *			 Filename:	msg_sender.cpp
 *
 *		Description:
 *
 *				Version:	1.0
 *				Created:	06.05.2017 16:12:13
 *			 Revision:	none
 *			 Compiler:	gcc
 *
 *				 Author:	YOUR NAME (),
 *	 Organization:
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <string.h>

#include "msg_sender.h"
#include "server_exceptions.h"

std::string Message::toString () const
{
	typedef std::map<std::string, std::string>::const_iterator MapIterator;
	std::ostringstream oss;
	oss << "##### " << commandName;
	for (MapIterator iter = params.begin(); iter != params.end(); iter++){
		oss << ", " << iter->first << ":" << iter->second;
	}
	oss << " #####\n";
	std::string str = oss.str();
	return str;
}

MessageSender::MessageSender()
{
	if( pipe(server_pipe) == -1 ) {
		std::cout<<"Pipe error!"<<std::endl;
	}

	if( pipe(threads_pipe) == -1 ) {
		std::cout<<"Pipe error!"<<std::endl;
	}

	auto func = std::bind( &MessageSender::updateSource, this, std::placeholders::_1 );
	source.setUpdateSourceFunction( func );
}

void MessageSender::sendMessage()
{
	Message m = popQueueMessage();
	sendMessage(m);
}

void MessageSender::sendMessage(Message& m)
{
	std::string str = m.toString();
	const char* msg = str.c_str();
	size_t all_bytes_sent = 0;
	size_t bytes_sent = 0;
	while( all_bytes_sent < strlen(msg)) {
		msg = msg + bytes_sent;
		bytes_sent = connection->send(msg, strlen(msg));
		all_bytes_sent += bytes_sent;
	}
}

Message MessageSender::popQueueMessage()
{
	std::unique_lock<std::mutex> lck(queue_mutex);
	if( msg_queue.empty() ) queue_empty.wait(lck);
	if( msg_queue.empty() ) throw ServerClosingSignal();
	Message m = msg_queue.front();
	msg_queue.pop();
	return m;
}

void MessageSender::pushQueueMessage(const Message& msg)
{
	std::unique_lock<std::mutex> lck(queue_mutex);
	msg_queue.emplace(msg);
	notifyAll();
}


Message MessageSender::getMessage ()
{
	Message m;
	getTypeToken( TokenSet( { Token::OBJECT_EDGE } ) );
	Token token = getTypeToken( TokenSet( { Token::STRING } ) );
	m.commandName = token.getValue();

	token = getTypeToken( TokenSet( { Token::OBJECT_EDGE, Token::SEPARATOR }) );
	while( token.getType() != Token::OBJECT_EDGE )
	{
		token = getTypeToken( TokenSet({ Token::STRING }) );
		std::string key = token.getValue();
		getTypeToken( TokenSet({ Token::KEY_VALUE_SEPARATOR }) );
		token = getTypeToken( TokenSet({ Token::STRING }) );
		std::string value = token.getValue();
		m.params[key] = value;
		token = getTypeToken( TokenSet( { Token::OBJECT_EDGE, Token::SEPARATOR }) );
	}

	return m;
}

void MessageSender::updateSource(BufferSource *s)
{
	const int len = 100;
	char buff [len];
	size_t bytes_got = connection->receive(buff, len);
	s->addToBuffer( buff, bytes_got );
}

Token MessageSender::getTypeToken( const TokenSet& t )
{
	Token token = lexer.getNextToken( source );
	if( t.count( token.getType() ) != 1 ) {
		throw UnexpectedToken();
	}

	return token;
}

void MessageSender::closeConnection()
{
	if( connection ) delete connection;
	connection = nullptr;
}

void MessageSender::stopServer() {
	char t = '!';
	write( server_pipe[1], &t, 1 );
	notifyAll();
}

void MessageSender::startServer() {
	char t;
	read( server_pipe[0], &t, 1 );
}

void MessageSender::stopAll() {
	char t = '!';
	write( threads_pipe[1], &t, 1 );
	notifyAll();
}

int MessageSender::waitForServerClosignEvent() {
	fd_set sdset;
	FD_ZERO(&sdset);
	FD_SET(threads_pipe[0], &sdset);
	FD_SET(0, &sdset);
	while( select( threads_pipe[0] + 1, &sdset, NULL, NULL, NULL ) <= 0 )
	{ }

	if( FD_ISSET(0, &sdset) ) {
		return 0;
	}

	if( FD_ISSET(threads_pipe[0], &sdset) ) {
		return threads_pipe[0];
	}

	return -1;
}

bool MessageSender::connect()
{
	connection = connector->connect(CONNECT_TIMEOUT, server_pipe[0]);
	connected = connection != nullptr;
	if( connected ) {
		connection->setPipe( server_pipe[0] );
		connection->setStoppingPipe( threads_pipe[0] );
	}
	return connected;
}
