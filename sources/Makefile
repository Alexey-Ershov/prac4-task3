HEADER=header.hpp
IMPL=impl.cpp
MAIN=main.cpp
OUTPUT=vm_deployer
PUGIDIR=./pugixml
INCLUDES=-I$(PUGIDIR)
CFLAGS=-O2 -std=c++17 -Wall
COMPILER=g++-9

.PHONY: all clean

all: main

impl.o: $(HEADER) $(IMPL)
	$(COMPILER) $(CFLAGS)  \
		        $(INCLUDES) \
	            -c \
	            -o impl.o \
	            $(IMPL)

pugi.o: $(PUGIDIR)/pugiconfig.hpp $(PUGIDIR)/pugixml.hpp $(PUGIDIR)/pugixml.cpp
	$(COMPILER) $(INCLUDES) \
	            -c \
	            -o pugi.o \
	            $(PUGIDIR)/pugixml.cpp

main.o: $(HEADER) $(MAIN)
	$(COMPILER) $(CFLAGS) \
	            -c \
	            -o main.o \
	            $(MAIN)

main: impl.o pugi.o main.o
	$(COMPILER) -o $(OUTPUT) impl.o pugi.o main.o

clean:
	rm -rf $(OUTPUT) *.o
