all: portf

portf: build/cli/main.o build/libportf.so
	gcc -o build/portf build/cli/main.o -L./build -lportf -lcurl -lcjson

build/cli/main.o: cli/main.c
	mkdir -pv build/cli
	gcc -Wall -I. -c cli/main.c -o build/cli/main.o

build/lib/portf.o: lib/portf.c lib/portf.h
	mkdir -pv build/lib
	gcc -c -Wall -Werror -fpic lib/portf.c -o build/lib/portf.o

build/libportf.so: build/lib/portf.o
	gcc -shared -o build/libportf.so build/lib/portf.o -lcurl -lcjson

.PHONY: test lint format db clean
test:
	nvim --headless --noplugin -u scripts/minimal_init.vim -c "PlenaryBustedDirectory tests/ { minimal_init = './scripts/minimal_init.vim' }"

lint:
	luacheck lua/portf

format:
	clang-format --style=file --dry-run -Werror cli/* lib/*

db:
	compiledb make

clean:
	rm -rf build compile_commands.json
