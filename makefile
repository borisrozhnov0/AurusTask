# Компилятор и настройки
CC=g++
INCLUDE_DIR=-I./include
SOURCES_DIR=source
OBJECTS_DIR=objects

SRCS=$(wildcard $(SOURCES_DIR)/*.cpp)
OBJS=$(patsubst $(SOURCES_DIR)/%.cpp, $(OBJECTS_DIR)/%.o, $(SRCS))
OBJS+=$(OBJECTS_DIR)/main.o

CFLAGS=-O3 -Wall
LDFLAGS=-std=c++17

TARGET = parser

all: $(TARGET)

$(TARGET): $(OBJS) 
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE_DIR) -o $@ $^ 

$(OBJECTS_DIR)/main.o: main.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE_DIR) -c $< -o $@ 

$(OBJECTS_DIR)/%.o: $(SOURCES_DIR)/%.cpp | $(OBJECTS_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE_DIR) -c $< -o $@ 

.PHONY: clean

clean:
	rm -f $(TARGET)
	rm -rf $(OBJS)
