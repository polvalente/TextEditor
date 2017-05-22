CC=g++
LD=g++

LIB=./lib
BIN=./bin
ETC=./etc

CFLAGS=-std=c++03 -Wall -I$(LIB) `perl -MExtUtils::Embed -e ccopts -e ccopts`
LFLAGS=-std=c++03 -I$(LIB) `perl -MExtUtils::Embed -e ccopts -e ldopts`

INCLUDES=-lncurses


OBJNAMES=interface.o\
     main.o\
     wrapper.o\
     editor.o\
     buffer.o

OBJS=$(addprefix $(ETC)/, $(OBJNAMES))

EXEC=TextEditor

$(ETC)/%.o : $(BIN)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

build: $(OBJS)
	$(LD) $(LFLAGS) -o $(EXEC) $^ $(INCLUDES) 

clean:
	rm -rf $(EXEC) $(OBJS) 
