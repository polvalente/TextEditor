CC=g++
LD=g++

RELEXE=TextEditor
DBGEXE=TextEditor-debug-build

LIB=./lib
BIN=./bin
#ETC=./etc
RELDIR=./release
DBGDIR=./debug

DIRLIST=$(LIB)\
	$(BIN)\
	$(RELDIR)\
	$(DBGDIR)

CFLAGS=-std=c++03 -Wall -I$(LIB) `perl -MExtUtils::Embed -e ccopts -e ccopts`
LFLAGS=-std=c++03 -I$(LIB) `perl -MExtUtils::Embed -e ccopts -e ldopts`

RELCFLAGS=-O2 -DNDEBUG
DBGCFLAGS=-g -O0 -DDEBUG

INCLUDES=-lncurses

OBJS=interface.o\
     main.o\
     wrapper.o\
     editor.o\
     buffer.o

RELOBJS=$(addprefix $(RELDIR)/, $(OBJS))
DBGOBJS=$(addprefix $(DBGDIR)/, $(OBJS))

.PHONY: all debug prep remake clean release

#.o rules
$(RELDIR)/%.o : $(BIN)/%.cpp
	$(CC) $(CFLAGS) $(RELCFLAGS) -c $< -o $@

$(DBGDIR)/%.o : $(BIN)/%.cpp
	$(CC) $(CFLAGS) $(DBGCFLAGS) -c $< -o $@


all: prep release

#debug rules
debug: $(DBGEXE)

$(DBGEXE): $(DBGOBJS)
	$(LD) -o $@ $^ $(INCLUDES) $(LFLAGS)

#release rules
release: $(RELEXE)

$(RELEXE): $(RELOBJS)
	$(LD) -o $@ $^ $(INCLUDES) $(LFLAGS)

#other rules
prep:
	@mkdir -p $(DIRLIST) # silently make dirs if they don't exist

remake: clean all

clean:
	rm -f $(RELEXE) $(DBGEXE) $(RELOBJS) $(DBGOBJS)
