CC = gcc
CXX = g++
ECHO = echo
RM = rm -f
TEX = pdflatex
TEXSOURCES = post_1.tex post_2.tex

CFLAGS = -Wall -Werror -ggdb3 -funroll-loops
CXXFLAGS = -std=c++20 -Wall -Werror -ggdb3 -funroll-loops
TEXFLAGS = -shell-escape -interaction=nonstopmode -file-line-error --interaction=batchmode

BIN = hw
OBJS = hw1.o

all: $(BIN) etags

$(BIN): $(OBJS)
	@$(ECHO) Linking $@
	@$(CC) $^ -o $@
	@$(ECHO) Generating Documentations $@
	@$(TEX) $(TEXFLAGS) $(TEXSOURCES) $@
-include $(OBJS:.o=.d)

%.o: %.c
	@$(ECHO) Compiling $<
	@$(CC) $(CFLAGS) -MMD -MF $*.d -c $<

%.o: %.cpp
	@$(ECHO) Compiling $<
	@$(CXX) $(CXXFLAGS) -MMD -MF $*.d -c $<

.PHONY: all clean clobber etags

clean:
	@$(ECHO) Removing compiled files
	@$(RM) *.o $(BIN) *.d TAGS core vgcore.* gmon.out

clobber: clean
	@$(ECHO) Removing all generated files
	@$(RM) *~ \#* *.txt *.pdf *.aux *.log

etags:
	@$(ECHO) Updating TAGS