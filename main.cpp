#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include "tcpconnector.h"
#include <pthread.h>
#include <thread>
#include <chrono>

#include "msg_sender.h"
#include "server_exceptions.h"

bool handle_camera;

void* cameraHandler( void * s )
{
	int i = 0;
	MessageSender *sender = (MessageSender*) s ;
	while( handle_camera )
	{
		Message m;
		m.commandName = "camera";
		m.params["count"] = std::to_string(++i);
		sender->pushQueueMessage(m);
		sender->notifyAll();
		std::this_thread::sleep_for(5s);
	}

}


void* send( void *  s)
{
	MessageSender *sender = (MessageSender*) s ;
	try {
		while( 1 )
		{
			try{
			sender->sendMessage();
			}
			catch( ServerDisconnected& e ) {
				std::cout<<"Server disconnected!"<<std::endl;
				return 0;
			}
		}
	}
	catch( ServerClosingSignal& e ) {
		std::cout<<"Received server closing signal"<<std::endl;
		return 0;
	}
}


void* receive( void * s)
{
	using namespace std::chrono_literals;
	MessageSender *sender = (MessageSender*) s;
	std::string address = "localhost";
	pthread_t send_thread;

	try {
		while( 1 )
		{
			while( !sender->connect( address.c_str(), 9995, 1 ) ) {
				std::this_thread::sleep_for(1s);
			}

			pthread_create(&send_thread, NULL, &send, sender);

			try {
				while(1)
				{
					Message m = sender->getMessage();
					std::cout<<m.toString()<<std::endl;
				}
			}
			catch(ServerDisconnected& e ){
				std::cout<<"Server disconnected!";
				sender->stopServer();
				pthread_join(send_thread, NULL);
				sender->startServer();
				sender->closeConnection();
			}
		}
	}
	catch( ServerClosingSignal& e ) {
		std::cout<<"Received server closing signal"<<std::endl;
		return 0;
	}
	return 0;
}

int main(int argc, char** argv)
{
	pthread_t receiver, camera;
	MessageSender sender;
	handle_camera = true;

	pthread_create(&receiver, NULL, &receive, &sender);
	pthread_create(&camera, NULL, &cameraHandler, &sender);

	char t;
	std::cin>>t;
	sender.stopServer();

	pthread_join(receiver, NULL);
	handle_camera = false;
	pthread_join(camera, NULL);

    exit(0);
}
