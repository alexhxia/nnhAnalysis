#!/bin/bash

nameMainFile=main

pdflatex "$nameMainFile".tex
pdflatex $nameMainFile.tex
biber $nameMainFile
pdflatex $nameMainFile.tex
pdflatex $nameMainFile.tex
