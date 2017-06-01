
#include <arpa/inet.h>
#include <iostream>
#include "tcpconnection.h"
#include "server_exceptions.h"

TCPConnection::TCPConnection(int sd) : m_sd(sd) {
}

TCPConnection::~TCPConnection()
{
    close(m_sd);
}

ssize_t TCPConnection::send(const char* buffer, size_t len)
{
	int buffer_ready = waitForWriteEvent();
	if( buffer_ready == -1 ) {
		throw ServerClosingSignal();
	}

	if( buffer_ready == m_pipe ) {
		throw ServerClosingSignal();
	}

	if( buffer_ready == stopping_pipe ) {
		throw ShutdownServerSignal();
	}

	int bytes_written = write( buffer_ready, buffer, len );
    return bytes_written;
}

ssize_t TCPConnection::receive(char* buffer, size_t len, int timeout)
{
	int buffer_ready = waitForReadEvent();
	if( buffer_ready == -1 ) {
		throw ServerClosingSignal();
	}

	if( buffer_ready == m_pipe ) {
		throw ServerClosingSignal();
	}

	if( buffer_ready == stopping_pipe ) {
		throw ShutdownServerSignal();
	}

	int bytes_read = read( buffer_ready, buffer, len );
	return bytes_read;
}

int TCPConnection::waitForReadEvent()
{
    fd_set sdset;
	fd_set errset;
	FD_ZERO(&errset);
	FD_SET(m_sd, &errset);
    FD_ZERO(&sdset);
    FD_SET(m_sd, &sdset);
	FD_SET(m_pipe, &sdset );
	FD_SET(stopping_pipe, &sdset);
    if (select(max_socket+1, &sdset, NULL, &errset, NULL) > 0)
    {
		if( FD_ISSET(m_pipe, &sdset) ) {
			return m_pipe;
		}

		if( FD_ISSET(stopping_pipe, &sdset) ) {
			return stopping_pipe;
		}

		if( FD_ISSET(m_sd, &sdset) ) {
			return m_sd;
		}

		if( FD_ISSET(m_sd, &errset) ) {
			return -1;
		}
    }
    return -1;
}

int TCPConnection::waitForWriteEvent()
{
    fd_set sdset;
	fd_set pipeset;
	fd_set errset;
	FD_ZERO(&errset);
	FD_SET(m_sd, &errset);
    FD_ZERO(&sdset);
    FD_SET(m_sd, &sdset);
	FD_ZERO(&pipeset);
	FD_SET(m_pipe, &pipeset );
	FD_SET(stopping_pipe, &pipeset);
    if ( select(max_socket+1, &pipeset, &sdset, &errset, NULL) > 0)
    {
		if( FD_ISSET(m_sd, &errset) ) {
			return -1;
		}

		if( FD_ISSET(m_sd, &sdset) ) {
			return m_sd;
		}

		if( FD_ISSET(stopping_pipe, &pipeset) ) {
			return stopping_pipe;
		}

		if( FD_ISSET(m_pipe, &pipeset) ) {
			return m_pipe;
		}
    }
    return -1;
}

int TCPConnection::getMaxSocket() 
{
	int max = 0;
	if( m_sd > max ) max = m_sd;
	if( m_pipe > max ) max = m_pipe;
	if( stopping_pipe > max ) max = stopping_pipe;
	return max;
}
