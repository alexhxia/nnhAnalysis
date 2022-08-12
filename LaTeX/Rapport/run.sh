#!/bin/bash

nameMainFile=Hocine

pdflatex "$nameMainFile".tex
pdflatex $nameMainFile.tex
bibtex $nameMainFile
#biber $nameMainFile
pdflatex $nameMainFile.tex
pdflatex $nameMainFile.tex
