INO=ino

all: rpi arduino

rpi:
	$(MAKE) -C rpi all

arduino: 
	$(MAKE) -C arduino all

clean:
	$(MAKE) -C rpi clean
	$(MAKE) -C arduino clean
