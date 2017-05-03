#ifndef __tcpconnection_h__
#define __tcpconnection_h__

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>

using namespace std;

class TCPConnection
{
    int     m_sd;
	int 	m_pipe;
	int 	max_socket;
    string  m_peerIP;
    int     m_peerPort;

  public:
    TCPConnection(int sd);

    ~TCPConnection();

    ssize_t send(const char* buffer, size_t len);
    ssize_t receive(char* buffer, size_t len, int timeout=0);

	void setPipe( int pipe )
	{ m_pipe = pipe; max_socket = m_pipe > m_sd ? m_pipe : m_sd; }

  private:
    int waitForReadEvent();
    int waitForWriteEvent();
 
    TCPConnection();
};

#endif
