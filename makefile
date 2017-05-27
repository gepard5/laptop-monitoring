CXX=g++
CFLAGS=-lpthread -Wall -std=c++14 `pkg-config --cflags --libs opencv` -lboost_program_options


DEPS=tcpconnection.h token.h lexer.h source.h bufferSource.h server_exceptions.h msg_sender.h
OBJ=main.o tcpconnection.o token.o lexer.o bufferSource.o msg_sender.o

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CFLAGS) 

monitor-movement: $(OBJ)
	$(CXX) -o $@ $^ $(CFLAGS)

clean:
	rm *.o
