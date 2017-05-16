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
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>


#include "msg_sender.h"
#include "server_exceptions.h"

MessageSender::MessageSender()
{
	if( pipe(server_pipe) == -1 ) {
		std::cout<<"Pipe error!"<<std::endl;
	}

	if( pipe(threads_pipe) == -1 ) {
		std::cout<<"Pipe error!"<<std::endl;
	}
}

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


void MessageSender::sendMessage ()
{
	Message m = popQueueMessage();
	std::string str = m.toString();
	const char* msg = str.c_str();
	size_t bytes_sent = connection->send(msg, str.length());
	if( bytes_sent <= 0 ) {
		throw ServerDisconnected();
	}
	else if( bytes_sent < str.length()) {
		size_t all_bytes_sent = bytes_sent;
		while(str.length() - all_bytes_sent > 0) {
			msg = msg + bytes_sent;
			bytes_sent = connection->send(msg, str.length() - all_bytes_sent);
			all_bytes_sent += bytes_sent;
	 	}
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

void MessageSender::updateSource()
{
	const int len = 100;
	char buff [len];
	size_t bytes_got = connection->receive(buff, len);

	if( bytes_got <= 0 ) {
		throw ServerDisconnected();
	}
	source.addToBuffer( buff, bytes_got );
}

Token MessageSender::getTypeToken( const TokenSet& t )
{
	Token token = lexer.getNextToken( source );
	while( token.getType() == Token::UNRECOGNISED ) {
		updateSource();
		token = lexer.getNextToken( source );
	}
	if( t.count( token.getType() ) != 1 ) {
		throw UnexpectedToken();
	}

	return token;
}


bool MessageSender::connect(const char* server, int port, int timeout)
{
    struct sockaddr_in address;

    memset (&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    if (resolveHostName(server, &(address.sin_addr)) != 0 ) {
        inet_pton(PF_INET, server, &(address.sin_addr));
    }

    long arg;
    fd_set sdset;
    struct timeval tv;
    socklen_t len;
    int result = -1, valopt, sd = socket(AF_INET, SOCK_STREAM, 0);

    if (sd < 0) {
		throw ServerClosingSignal();
        return false;
    }

    arg = fcntl(sd, F_GETFL, NULL);
    arg |= O_NONBLOCK;
    fcntl(sd, F_SETFL, arg);

	std::string message;
    if ((result = ::connect(sd, (struct sockaddr *)&address, sizeof(address))) < 0)
    {
        if (errno == EINPROGRESS)
        {
			fd_set pipeset;
            tv.tv_sec = timeout;
            tv.tv_usec = 0;
            FD_ZERO(&sdset);
            FD_SET(sd, &sdset);
			FD_ZERO(&pipeset);
			FD_SET(server_pipe[0], &pipeset);

			int max_s = sd > server_pipe[0] ? sd : server_pipe[0];

            int s = -1;
            do {
                s = select(max_s+1, &pipeset, &sdset, NULL, &tv);
            } while (s == -1 && errno == EINTR);

			if( FD_ISSET(server_pipe[0], &pipeset) ) {
				throw ServerClosingSignal();
			}

            if (s > 0)
            {
                len = sizeof(int);
                getsockopt(sd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &len);
                if (valopt) {
                    fprintf(stderr, "connect() error %d - %s\n", valopt, strerror(valopt));
                }
                else {
					result = 0;
				}
            }
        }
    }

	if (result == -1) return false;
    connection = new TCPConnection(sd);
	connection->setPipe( server_pipe[0] );
	connection->setStoppingPipe( threads_pipe[0] );
	return true;
}

void MessageSender::closeConnection()
{
	if( connection ) delete connection;
	connection = nullptr;
}

int MessageSender::resolveHostName(const char* hostname, struct in_addr* addr)
{
    struct addrinfo *res;

    int result = getaddrinfo (hostname, NULL, NULL, &res);
    if (result == 0) {
        memcpy(addr, &((struct sockaddr_in *) res->ai_addr)->sin_addr, sizeof(struct in_addr));
        freeaddrinfo(res);
    }
    return result;
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



