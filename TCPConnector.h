/*
 * =====================================================================================
 *
 *       Filename:  TCPConnector.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01.06.2017 19:05:23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef TCPCONNECTOR
#define TCPCONNECTOR

#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/in.h>
#include "tcpconnection.h"
#include "server_exceptions.h"

class TCPConnector {
	public:
		TCPConnection *connect(int, int);
		int resolveHostName(const char*, struct in_addr*);

		int getPort();
		void setPort( int p);
		std::string getServerAddress();
		void setServerAddress(std::string s);

	private:
		std::string server_address{ "localhost" };
		int port{ 9001 };
};

#endif
