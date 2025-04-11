ifeq ($(OS),Windows_NT)
	TARGET_OS = Windows
else ifeq ($(WIN_BUILD),1)
	TARGET_OS = Windows
	CROSS_TOOLCHAIN = x86_64-w64-mingw32-
endif

ifeq ($(TARGET_OS),Windows)
	EXT = .exe
endif

CC = $(CROSS_TOOLCHAIN)gcc
STRIP = $(CROSS_TOOLCHAIN)strip

TARGET = crc32$(EXT)
CFLAGS = -O2 -Wall

SRCS = ./crc32.c

all:
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)
	@$(STRIP) $(TARGET)

clean:
ifeq ($(OS),Windows_NT)
	@del /q $(TARGET).o $(TARGET) 2>NUL
else
	$(RM) $(TARGET).o $(TARGET)
endif
