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

main: impl pugi main_obj
	$(COMPILER) -o $(OUTPUT) impl.o pugi.o main.o

impl: $(HEADER) $(IMPL)
	$(COMPILER) $(CFLAGS)  \
		        $(INCLUDES) \
	            -c \
	            -o impl.o \
	            $(IMPL)

pugi: $(PUGIDIR)/pugiconfig.hpp $(PUGIDIR)/pugixml.hpp $(PUGIDIR)/pugixml.cpp
	$(COMPILER) $(INCLUDES) \
	            -c \
	            -o pugi.o \
	            $(PUGIDIR)/pugixml.cpp

main_obj: $(HEADER) $(MAIN)
	$(COMPILER) $(CFLAGS) \
	            -c \
	            -o main.o \
	            $(MAIN)

clean:
	rm $(OUTPUT) *.o
