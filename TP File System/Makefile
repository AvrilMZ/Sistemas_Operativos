CC = gcc
CFLAGS := -ggdb3 -O2 -Wall -std=c11 -Wno-unused-function -Wvla -D_FILE_OFFSET_BITS=64

SRCDIR = src

# Flags for FUSE
LDLIBS := $(shell pkg-config fuse --cflags --libs)

# Name for the filesystem!
FS_NAME := fisopfs

SRCS := fisopfs.c \
        src/filesystem.c \
        src/inode.c \
        src/bitmap.c \
        src/directory.c

OBJS := $(SRCS:.c=.o)

all: $(FS_NAME)

$(FS_NAME): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	
format: .clang-files .clang-format
	xargs -r clang-format -i <$<

build:
	./dock build

run:
	./dock run

exec:
	./dock exec

clean:
	rm -rf $(EXEC) *.o $(SRCDIR)/*.o core vgcore.* $(FS_NAME) test

.PHONY: all build clean format docker-build docker-run docker-exec
