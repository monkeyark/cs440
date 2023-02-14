ECHO = echo
RM = rm -f
TEX = pdflatex

TEXFLAGS = -shell-escape -interaction=nonstopmode -file-line-error --interaction=batchmode

TEXFILE = developers.tex

all: $(TEXFILE)
	@$(ECHO) Generating Documentations $@
	@$(TEX) $(TEXFLAGS) $(TEXFILE) $@

clean:
	@$(ECHO) Removing generated documentations
	@$(RM) *.pdf *.aux *.log *.fls *.fdb_latexmk *.gz

clobber: clean
	@$(ECHO) Removing all generated files
	@$(RM) *~ \#* *.txt

.PHONY: all clean clobber