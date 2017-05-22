CC=g++
LD=g++

CFLAGS=-Wall `perl -MExtUtils::Embed -e ccopts -e ccopts`
LFLAGS=`perl -MExtUtils::Embed -e ccopts -e ldopts`

INCLUDES=-lncurses

OBJS=interface.o\
     main.o\
     wrapper.o\
     editor.o\
     buffer.o

EXEC=TextEditor


%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

build: $(OBJS)
	$(LD) $(LFLAGS) -o TextEditor $^ $(INCLUDES) 

clean:
	rm -rf $(EXEC) $(OBJS) 
