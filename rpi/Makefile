CC=g++ -std=c++0x
CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix src/,$(notdir $(CPP_FILES:.cpp=.o)))
LIBS="-lwiringPi"

all: copy-framework

copy-framework:
	find ../framework/rpi/ -name '*.h' -exec cp {} src/ \;
	find ../framework/rpi/ -name '*.cpp' -exec cp {} src/ \;
	find ../framework/commons/ -name '*.cpp' -exec cp {} src/ \;
	find ../framework/commons/ -name '*.h' -exec cp {} src/ \;
	make objects

objects: $(OBJ_FILES)
	$(CC) -o main $(OBJ_FILES) $(LIBS)

src/%.o: src/%.cpp
	$(CC) -c -o $@ $<

clean:
	-@rm -rf src/*.o || true
	-@rm main || true
	-@rm `fdupes -f -1 -r src/ ../framework/` || true
