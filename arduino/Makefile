INO=ino

all: arduino

arduino: copy-framework arduino-build arduino-upload

copy-framework:
	find ../framework/arduino/ -name '*.h' -exec cp {} src/ \;
	find ../framework/arduino/ -name '*.cpp' -exec cp {} src/ \;
	find ../framework/commons/ -name '*.cpp' -exec cp {} src/ \;
	find ../framework/commons/ -name '*.h' -exec cp {} src/ \;

arduino-build:
	$(INO) build;

arduino-upload:
	$(INO) upload;

clean:
	$(INO) clean	
	-@rm `fdupes -r ../framework/ src/ | grep -v frame| grep src` 2> /dev/null || true
