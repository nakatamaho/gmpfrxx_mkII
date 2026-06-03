TARGET = gmpfrxx_mkII_manual
TEX = $(TARGET).tex
PDF = $(TARGET).pdf
PDFLATEX ?= pdflatex

.PHONY: all clean distclean

all: $(PDF)

$(PDF): $(TEX)
	$(PDFLATEX) -interaction=nonstopmode -halt-on-error $(TEX)
	$(PDFLATEX) -interaction=nonstopmode -halt-on-error $(TEX)
	$(PDFLATEX) -interaction=nonstopmode -halt-on-error $(TEX)

clean:
	rm -f $(TARGET).aux $(TARGET).log $(TARGET).out $(TARGET).toc

distclean: clean
	rm -f $(PDF)
