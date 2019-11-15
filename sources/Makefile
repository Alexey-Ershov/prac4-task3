HEADER=header.hpp
IMPL=impl.cpp
MAIN=main.cpp
OUTPUT=vm_deployer
PUGIDIR=./pugixml
INCLUDES=-I$(PUGIDIR)
CFLAGS=-O2 -Wall -Werror -std=c++11

.PHONY: all clean

all: main

main: impl pugi main_obj
	g++ -o $(OUTPUT) impl.o pugi.o main.o

impl: $(HEADER) $(IMPL)
	g++ $(CFLAGS) \
		$(INCLUDES) \
	    -c \
	    -o impl.o \
	    $(IMPL)

pugi: $(PUGIDIR)/pugiconfig.hpp $(PUGIDIR)/pugixml.hpp $(PUGIDIR)/pugixml.cpp
	g++ $(INCLUDES) \
	    -c \
	    -o pugi.o \
	    $(PUGIDIR)/pugixml.cpp

main_obj: $(HEADER) $(MAIN)
	g++ $(CFLAGS) \
	    -c \
	    -o main.o \
	    $(MAIN)

clean:
	rm $(OUTPUT) *.o
