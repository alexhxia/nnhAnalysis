#!/bin/bash

pdflatex notes.tex
pdflatex notes.tex
biber notes
pdflatex notes.tex
pdflatex notes.tex
