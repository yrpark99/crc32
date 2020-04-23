ifeq ($(OS),Windows_NT)
	EXT = .exe
endif

CC = gcc
STRIP = strip
RM = rm -f

TARGET = crc32$(EXT)
CFLAGS = -O2 -Wall

SRCS = ./crc32.c

all:
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)
	@$(STRIP) $(TARGET)

clean:
	$(RM) $(TARGET).o $(TARGET)
