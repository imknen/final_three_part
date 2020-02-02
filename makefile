.PHONY: clean 

CFLAGS= -Wall -std=c++1z -O3

all: main.o parse.o search_server.o 
	g++ -o test $(CFLAGS) $^
main.o:	main.cpp
	g++ -c $(CFLAGS) $^
parse.o:	parse.cpp parse.h
	g++ -c $(CFLAGS) $<
search_server.o:	search_server.cpp search_server.h
	g++ -c $(CFLAGS) $<
clean:
	rm -f *.o test
