CC=/tools/emu/emu-19.10/bin/emu-cc
CFILES=$(wildcard *.c)

all:
	$(CC) -o firehose.mwx $(CFILES)

clean:
	rm -f firehose.mwx
	rm -f firehose.cdc
	rm -f firehose.hdd
	rm -f firehose.vsf
