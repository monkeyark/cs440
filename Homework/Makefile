CC = gcc
CXX = g++
ECHO = echo
RM = rm -f
TEX = pdflatex

CFLAGS = -Wall -Werror -ggdb3 -funroll-loops
CXXFLAGS = -std=c++20 -Wall -Werror -ggdb3 -funroll-loops
TEXFLAGS = -shell-escape -interaction=nonstopmode -file-line-error --interaction=batchmode

BIN = hw
OBJS = hw1.o
TEXFILES = post_3.tex

all: $(BIN) etags

$(BIN): $(OBJS)
	@$(ECHO) Linking $@
	@$(CC) $^ -o $@
-include $(OBJS:.o=.d)

%.o: %.c
	@$(ECHO) Compiling $<
	@$(CC) $(CFLAGS) -MMD -MF $*.d -c $<

%.o: %.cpp
	@$(ECHO) Compiling $<
	@$(CXX) $(CXXFLAGS) -MMD -MF $*.d -c $<

doc: $(TEXFILES)
	@$(ECHO) Generating Documentations $@
	@$(TEX) $(TEXFLAGS) $(TEXFILES) $@

clean:
	@$(ECHO) Removing compiled files
	@$(RM) *.o $(BIN) *.d TAGS core vgcore.* gmon.out

cleandoc:
	@$(ECHO) Removing generated documentations
	@$(RM) *.pdf *.aux *.log *.fls *.fdb_latexmk *.gz

clobber: clean cleandoc
	@$(ECHO) Removing everything
	@$(RM) *~ \#*

etags:
	@$(ECHO) Updating TAGS

.PHONY: all clean cleandoc clobber etags