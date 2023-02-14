CC = gcc
CXX = g++
ECHO = echo
RM = rm -f

CFLAGS = -Wall -Werror -ggdb3 -funroll-loops
CXXFLAGS = -std=c++20 -Wall -Werror -ggdb3 -funroll-loops

BIN = mycc
OBJS = mycc.o lex.o

all: $(BIN) etags

$(BIN): $(OBJS)
	@$(ECHO) Linking $@
	@$(CXX) $^ -o $@
-include $(OBJS:.o=.d)

%.o: %.c
	@$(ECHO) Compiling $<
	@$(CC) $(CFLAGS) -MMD -MF $*.d -c $<

%.o: %.cpp
	@$(ECHO) Compiling $<
	@$(CXX) $(CXXFLAGS) -MMD -MF $*.d -c $<

doc: $(TEXFILE)
	@$(ECHO) Generating Documentations $@
	@$(TEX) $(TEXFLAGS) $(TEXFILE) $@

clean:
	@$(ECHO) Removing compiled files
	@$(RM) *.o $(BIN) *.d TAGS core vgcore.* gmon.out

clobber: clean
	@$(ECHO) Removing all generated files
	@$(RM) *~ \#* *.txt *.pdf *.aux *.log

etags:
	@$(ECHO) Updating TAGS

.PHONY: all clean clobber etags