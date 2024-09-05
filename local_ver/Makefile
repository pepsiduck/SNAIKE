CC=g++
CFLAGS=-Wall -Werror -g
INCLUDE_HEADERS_DIRECTORY=-Iheaders

SNAIKE: main.cpp src/train.cpp src/AI.cpp
	$(CC) $(INCLUDE_HEADERS_DIRECTORY) $(CFLAGS) -o $@ $^ 

%.o: %.cpp                  
	$(CC) $(INCLUDE_HEADERS_DIRECTORY) $(CFLAGS) -o $@ -cpp $<

clean:
	rm -f src/*.o
	rm -f SNAIKE

.PHONY: clean tests
