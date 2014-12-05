rm doc.glsdefs
pdflatex doc.tex
pdflatex doc.tex
makeindex -s doc.ist -o doc.gls doc.glo
makeindex -s doc.ist -o doc.acr doc.acn
pdflatex doc.tex
