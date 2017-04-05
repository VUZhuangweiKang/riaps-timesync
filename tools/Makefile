SRC = $(wildcard *.c)
BIN = $(patsubst %.c,%,$(SRC))

LDLIBS += -lsoc -lm
CFLAGS += -Wall

all : $(BIN)

clean :
	rm -f $(BIN)

.PHONY: all clean

