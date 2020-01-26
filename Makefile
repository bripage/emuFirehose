CC=/tools/emu/emu-19.10/bin/emu-cc
CFILES=$(wildcard *.c)

all:
	$(CC) -o spmv.mwx $(CFILES)	

clean:
	rm -f spmv.mwx
	rm -f spmv.cdc
	rm -f spmv.hdd
	rm -f spmv.vsf
