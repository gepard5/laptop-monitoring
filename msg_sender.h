/*
 * =====================================================================================
 *
 *       Filename:  msg_sender.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06.05.2017 16:12:25
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */

#include <map>
#include <set>
#include <queue>
#include <string>
#include <sstream>
#include <mutex>
#include <condition_variable>
#include <netinet/in.h>
#include "tcpconnection.h"
#include "bufferSource.h"
#include "lexer.h"

struct Message {
	std::string toString() const;

 	std::string commandName;
 	std::map<std::string, std::string> params;
};


class MessageSender {
	public:
 		MessageSender();
		~MessageSender()
		{ if(connection) delete connection; }

		bool connect(int);
		bool isConnected() const
		{ return connected; }
		void closeConnection();
		int resolveHostName(const char*, struct in_addr*);

 		void sendMessage();
		void sendMessage(Message& m);
 		Message getMessage();

		Message popQueueMessage();
		void pushQueueMessage(const Message&);

		void startServer();
		void stopServer();
		void stopAll();

		int waitForServerClosignEvent();

		int getPort();
		void setPort( int p);
		std::string getServerAddress();
		void setServerAddress(std::string s);
 	private:
		typedef std::set<Token::TYPE> TokenSet;

		Token getTypeToken( const TokenSet& );
		void updateSource(BufferSource*);

		void notifyAll()
		{ queue_empty.notify_all(); }

		int server_pipe[2];
		int threads_pipe[2];
 		std::queue<Message> msg_queue;
		std::mutex queue_mutex;
		std::condition_variable queue_empty;
 		TCPConnection* connection;
		BufferSource source;
		Lexer lexer;
		std::string server_address{ "localhost" };
		int port{ 9001 };
		bool connected{ false };
};
