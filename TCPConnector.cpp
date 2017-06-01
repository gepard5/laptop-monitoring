/*
 * =====================================================================================
 *
 *       Filename:  TCPConnector.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01.06.2017 19:07:48
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "TCPConnector.h"

TCPConnection* TCPConnector::connect(int timeout, int server_pipe)
{
    struct sockaddr_in address;
		const char * server = server_address.c_str();

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
        return nullptr;
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
			FD_SET(server_pipe, &pipeset);

			int max_s = sd > server_pipe ? sd : server_pipe;

            int s = -1;
            do {
                s = select(max_s+1, &pipeset, &sdset, NULL, &tv);
            } while (s == -1 && errno == EINTR);

			if( FD_ISSET(server_pipe, &pipeset) ) {
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

	if (result == -1) {
		return nullptr;
	}
    
	return new TCPConnection(sd);
}

int TCPConnector::resolveHostName(const char* hostname, struct in_addr* addr)
{
    struct addrinfo *res;

    int result = getaddrinfo (hostname, NULL, NULL, &res);
    if (result == 0) {
        memcpy(addr, &((struct sockaddr_in *) res->ai_addr)->sin_addr, sizeof(struct in_addr));
        freeaddrinfo(res);
    }
    return result;
}

int TCPConnector::getPort() {
	return port;
}

void TCPConnector::setPort (int p) {
	port = p;
}

std::string TCPConnector::getServerAddress () {
	return server_address;
}

void TCPConnector::setServerAddress (std::string s) {
	server_address = s;
}
