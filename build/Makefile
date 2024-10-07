VPATH=../src:../tests

DEBUG=-DDEBUG -g
CFLAGS=-Wall -O3 -std=c++11 $(DEBUG)
LDFLAGS=
CC=gcc
CXX=g++
#CC=i686-w64-mingw32-gcc
#CXX=i686-w64-mingw32-g++

OBJECTS= \
  Elf.o

default: $(OBJECTS)
	$(CXX) -o ../anything2obj ../src/anything2obj.cpp $(OBJECTS) \
	  $(CFLAGS) $(LDFLAGS)

%.o: %.cpp %.h
	$(CXX) -c $< -o $*.o $(CFLAGS)

