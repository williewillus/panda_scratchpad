CC =  gcc
G++ = g++
OPT = -std=gnu++11
BUILDDIR = $(CURDIR)/../build
ROOTDIR = ../

TARGET = $(ROOTDIR)harness

all: \
	$(TARGET)

$(BUILDDIR): \
	mkdir -p $@

$(TARGET): \
	$(ROOTDIR)main.cpp \
	ClientSocket.cpp \
	$(ROOTDIR)FSCommands.cpp
	$(G++) $(OPT) $^ -ldl -o $@


