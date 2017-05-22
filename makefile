CC=g++
LD=g++

LIB=./lib
BIN=./bin
ETC=./etc

CFLAGS=-Wall -I$(LIB) `perl -MExtUtils::Embed -e ccopts -e ccopts`
LFLAGS=-I$(LIB) `perl -MExtUtils::Embed -e ccopts -e ldopts`

INCLUDES=-lncurses


OBJNAMES=interface.o\
     main.o\
     wrapper.o\
     editor.o\
     buffer.o

OBJS=$(addprefix $(ETC)/, $(OBJNAMES))

EXEC=$(ETC)/TextEditor

$(ETC)/%.o : $(BIN)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

build: $(OBJS)
	$(LD) $(LFLAGS) -o $(ETC)/TextEditor $^ $(INCLUDES) 
	echo "./$(EXEC)" > TextEditor
	chmod +x TextEditor

clean:
	rm -rf $(EXEC) $(OBJS) 
