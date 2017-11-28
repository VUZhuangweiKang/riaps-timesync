SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o) 
BIN = sync_gen 

LDLIBS += -lm
CFLAGS += -Wall

all : $(BIN)

clean :
	rm -f $(BIN) $(OBJS) ./.depend

.PHONY: all clean depend

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CC) $(CFLAGS) -MM $^ -MF  ./.depend

include .depend
