SHELL 				= /bin/sh
CC					= g++

FLAGS				= -std=gnu11 -Iinclude
CFLAGS				= -pedantic -c -Wextra -Wall -ggdb3
DEBUGFLAGS 			= -O0 -D _DEBUG
RELEASEFLAGS 		= -O2 -D NDEBUG -combine -fwhole-program

TARGET 				= MKPA
SOURCES				= $(shell find . -regex ".*\.\(h\|cpp\)")
COMMON 				= 
HEADERS				= $(shell echo include/*h)
OBJECTS 			= $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS) $(COMMON)
	$(CC) $(FLAGS) $(CFLAGS) $(DEBUGFLAGS) -o $(TARGET) $(OBJECTS)
	
release: $(SOURCES) $(HEADERS) $(COMMON)
	$(CC)	$(FLAGS)	$(CFLAGS)	$(RELEASEFLAGS)	-o	$(TARGET)	$(SOURCES)