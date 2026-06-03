PAPER=pubchemqc_wB97XD
SI=supporting_information
DATE=$(shell date "+%m%d")
DESKTOP=$(HOME)/Desktop

FIGURES=fig_koopmans.pdf fig_sf.pdf fig_tadf.pdf
INPUTS=koopmans_paragraph.tex tab_koopmans.tex

.PHONY: all clean zip diff watch date

all: $(SI).pdf $(PAPER).pdf
	mkdir -p $(DESKTOP)
	cp $(PAPER).pdf $(SI).pdf $(DESKTOP)/

zip: all
	rm -rf tmp
	mkdir -p tmp
	cp $(PAPER).pdf $(SI).pdf Makefile *.bib *.tex *.pdf tmp/
	cd tmp ; rm -f *~ \#* l ll ; rm -f ../$(PAPER)_$(DATE).zip $(PAPER)_$(DATE).zip ; zip -r ../$(PAPER)_$(DATE).zip * ; cd .. ; rm -rf tmp

clean:
	rm -rf tmp $(PAPER).pdf $(PAPER).dvi $(PAPER).aux $(PAPER).log $(PAPER).rtf report_$(PAPER).html $(PAPER).txt $(PAPER).bbl $(PAPER).blg $(PAPER)-diff.* $(SI).pdf $(SI).dvi $(SI).aux $(SI).log $(SI).rtf report_$(SI).html $(SI).txt $(SI).bbl $(SI).blg *blg *bbl *aux *log *~ acs-*

$(PAPER).pdf: $(PAPER).tex $(PAPER).bib $(INPUTS) $(FIGURES) $(SI).aux
	pdflatex $(PAPER).tex
	pdflatex $(PAPER).tex
	bibtex $(PAPER)
	pdflatex $(PAPER).tex
	pdflatex $(PAPER).tex

$(SI).aux: $(SI).tex
	pdflatex $(SI).tex

$(SI).pdf: $(SI).tex
	pdflatex $(SI).tex
	pdflatex $(SI).tex

diff: $(PAPER)-diff.pdf

$(PAPER)-diff.tex: $(PAPER).tex
	latexdiff-vc --flatten -t CFONT ../1st_ver/$(PAPER).tex $(PAPER).tex > $(PAPER)-diff.tex

$(PAPER)-diff.pdf: $(PAPER)-diff.tex $(PAPER).bib $(INPUTS) $(FIGURES)
	pdflatex $(PAPER)-diff.tex
	bibtex $(PAPER)-diff
	pdflatex $(PAPER)-diff.tex
	pdflatex $(PAPER)-diff.tex

watch:
	latexmk -pdf -pvc $(PAPER).tex

date:
	echo $(DATE)
