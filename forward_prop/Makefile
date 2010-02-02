#all: nfr1.dot nfr2.dot nfr3.dot nfr4.dot nfr-and.dot nfr-or.dot nfr-w+.dot
all: AutoManufacturerNP.dot ContAutoManufacturerNP.dot

nfr%.dot:nfr%.model nfr.py
	cp nfr$*.model nfr.model
	python nfr.py > nfr$*.dot
	rm -f nfr.model

%.dot:%.dat
	cp $*.dat nfr.dat
	python model.py > $*.dot
	rm -f nfr.dat
	
clean:
	rm -f *.dot
