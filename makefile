all: main.o parse.o search_server.o
	g++ -std=c++1z -pthread -o test main.o parse.o search_server.o
main.o:	main.cpp
	g++ -std=c++1z -pthread -c main.cpp
parse.o:	parse.h parse.cpp
	g++ -std=c++1z -pthread -c parse.cpp
search_server.o:	search_server.h search_server.cpp
	g++ -std=c++1z -pthread -c search_server.cpp
clean:
	rm -f *.o test
