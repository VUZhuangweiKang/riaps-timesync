SRC = $(wildcard *.c)
BIN = $(patsubst %.c,%,$(SRC))

LDLIBS += -lsoc
CFLAGS += -Wall

all : $(BIN)

clean :
	rm -f $(BIN)

.PHONY: all clean

