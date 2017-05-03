
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
	if( buffer_ready == m_pipe ) {
		throw ServerDisconnected();
	}

	int bytes_written = write( buffer_ready, buffer, len );
    return bytes_written;
}

ssize_t TCPConnection::receive(char* buffer, size_t len, int timeout)
{
	int buffer_ready = waitForReadEvent();
	if( buffer_ready == m_pipe ) {
		throw ServerDisconnected();
	}

	int bytes_read = read( buffer_ready, buffer, len );
	return bytes_read;
}

int TCPConnection::waitForReadEvent()
{
    fd_set sdset;
    FD_ZERO(&sdset);
    FD_SET(m_sd, &sdset);
	FD_SET(m_pipe, &sdset );
    if (select(max_socket+1, &sdset, NULL, NULL, NULL) > 0)
    {
		if( FD_ISSET(m_pipe, &sdset) ) {
			return m_pipe;
		}

		if( FD_ISSET(m_sd, &sdset) ) {
			return m_sd;
		}
    }
    return 0;
}

int TCPConnection::waitForWriteEvent()
{
    fd_set sdset;
	fd_set pipeset;
    FD_ZERO(&sdset);
    FD_SET(m_sd, &sdset);
	FD_ZERO(&pipeset);
	FD_SET(m_pipe, &pipeset );
    if (select(max_socket+1, &sdset, &pipeset, NULL, NULL) > 0)
    {
		if( FD_ISSET(m_sd, &sdset) ) {
			return m_sd;
		}

		if( FD_ISSET(m_pipe, &sdset) ) {
			return m_pipe;
		}
    }
    return 0;
}
