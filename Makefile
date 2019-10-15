TARG   = nate
OFILES = util.o main.o rope.o cmd.o
HFILES = nate.h
CFLAGS = -std=c89 -pedantic -Wall 
CC = clang

$(TARG) : $(OFILES)
	$(CC) -o $@ $(LDFLAGS) $(OFILES)

$(OFILES) : $(HFILES)

clean :
	rm -f $(TARG) $(OFILES)
