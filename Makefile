CFLAGS = -Wall -Wextra -Werror

build:
	mkdir -p build

build/kanacli:
	g++ ./src/kanacli.cpp -o ./build/kanacli $(CFLAGS)

all: build build/kanacli
clean:
	rm -rf build
