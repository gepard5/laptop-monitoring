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

#include "tcpconnection.h"
#include "TCPConnector.h"
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

 		void sendMessage();
		void sendMessage(Message& m);
 		Message getMessage();

		Message popQueueMessage();
		void pushQueueMessage(const Message&);

		void startServer();
		void stopServer();
		void stopAll();

		bool connect();
		bool isConnected() const
		{ return connected; }
		void setConnector( TCPConnector* tcp )
		{ connector = tcp; }
		void closeConnection();

		int waitForServerClosignEvent();

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

		TCPConnector *connector;
 		TCPConnection *connection;
		bool connected;
		const int CONNECT_TIMEOUT = 1;

		BufferSource source;
		Lexer lexer;
};
