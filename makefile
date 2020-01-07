.PHONY: all clean 

CFLAGS= -c -Wall -pthread -std=c++1z -g3

all: main.o parse.o search_server.o
	g++ -g3 -std=c++1z -pthread -o test main.o parse.o search_server.o
main.o:	main.cpp
	g++ $(CFLAGS) $^
parse.o:	parse.cpp parse.h
	g++ $(CFLAGS) $<
search_server.o:	search_server.cpp search_server.h
	g++ $(CFLAGS) $<
clean:
	rm -f *.o test
