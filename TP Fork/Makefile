CFLAGS := -ggdb3 -O2 -Wall -Wextra -std=c11
CFLAGS += -Wvla
CPPFLAGS := -D_DEFAULT_SOURCE

SRCDIR_UNIX := challenge_unix
SRCDIR_STRACE := challenge_strace
PROGS := primes xargs cp ls ps strace

all: $(PROGS)

xargs: xargs.o

primes: primes.o

cp: $(SRCDIR_UNIX)/cp.o
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^

ls: $(SRCDIR_UNIX)/ls.o
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^

ps: $(SRCDIR_UNIX)/ps.o
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^

strace: $(SRCDIR_STRACE)/strace.o
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^

test:
	./tests/run

format: .clang-files .clang-format
	xargs -r clang-format -i <$<

clean:
	rm -f $(PROGS) *.o $(SRCDIR_UNIX)/*.o $(SRCDIR_STRACE)/*.o core vgcore.*

.PHONY: all clean format test