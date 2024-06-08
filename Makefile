CC = g++
CFLAGS = -Wall -W -std=c++17 -Wextra -Wno-unused-parameter -pthread
LDFLAGS = 

SRCS = main/main.cpp main/func.cpp
OBJS = $(SRCS:.cpp=.o)
DEPS = main/header.h
TARGET = archiver
HOFMAN_SRCS = hofman/hofman.cpp hofman/hofmanFunc.cpp
HOFMAN_OBJS = $(HOFMAN_SRCS:.cpp=.o)
HOFMAN_DEPS = hofman/hofmanHeader.h
HOFMAN_TARGET = hofmanArchiver
LZW_SRCS = LZW/lzw.cpp LZW/lzwFunc.cpp
LZW_OBJS = $(LZW_SRCS:.cpp=.o)
LZW_DEPS = LZW/lzwHeader.h
LZW_TARGET = lzwArchiver
LZ77_SRCS = LZ77/lz77.cpp LZ77/lz77Func.cpp
LZ77_OBJS = $(LZ77_SRCS:.cpp=.o)
LZ77_DEPS = LZ77/lz77Header.h
LZ77_TARGET = lz77Archiver

.PHONY: all clean run

all: $(TARGET) $(HOFMAN_TARGET) $(LZW_TARGET) $(LZ77_TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(HOFMAN_TARGET): $(HOFMAN_OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(LZW_TARGET): $(LZW_OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(LZ77_TARGET): $(LZ77_OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.cpp $(DEPS) $(HOFMAN_DEPS) $(LZW_DEPS) $(LZ77_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) $(HOFMAN_OBJS) $(HOFMAN_TARGET) $(LZW_OBJS) $(LZW_TARGET) $(LZ77_OBJS) $(LZ77_TARGET)

run:
	./$(TARGET) --help
	./$(TARGET) -a -f myfile.txt -n myarchive
	./$(TARGET) -u -f myarchive.archive
