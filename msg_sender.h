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

		bool connect( const char*, int, int);
		void closeConnection();
		int resolveHostName(const char*, struct in_addr*);

 		void sendMessage();
 		Message getMessage();
		Message popQueueMessage();
		void pushQueueMessage(const Message&);
		void updateSource();

		void startServer();
		void stopServer();

		void notifyAll()
		{ queue_empty.notify_all(); }

 	private:
		typedef std::set<Token::TYPE> TokenSet;

		Token getTypeToken( const TokenSet& );

		int server_pipe[2];
		int threads_pipe[2];
 		std::queue<Message> msg_queue;
		std::mutex queue_mutex;
		std::condition_variable queue_empty;
 		TCPConnection* connection;
		BufferSource source;
		Lexer lexer;
};
