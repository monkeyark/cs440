ECHO = echo
RM = rm -f
TEX = pdflatex

TEXFLAGS = -shell-escape -interaction=nonstopmode -file-line-error --interaction=batchmode

TEXFILES = post_4.tex

all: $(TEXFILES)
	@$(ECHO) Generating Documentations $@
	@$(TEX) $(TEXFLAGS) $(TEXFILES) $@

clean:
	@$(ECHO) Removing generated documentations
	@$(RM) *.pdf *.aux *.log *.fls *.fdb_latexmk *.gz

.PHONY: all clean