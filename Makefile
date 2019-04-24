NOECHO=@

c_files=lexical_analyzer main simple_list token_analyzer
build_objects=$(patsubst %, build/%.o, $(c_files))
src_files=$(patsubst %, src/%.c, $(c_files))

help:
	$(NOECHO)echo "Type \"make build\" if you want to"

build/%.o: src/%.c build_dir
	$(CC) -O2 -std=c99 -g -Wall -pedantic -c $< -o $@

bin/converter: bin_dir
	$(CC) $(build_objects) -o $@

build: $(build_objects) bin/converter
	$(NOECHO)echo Done!

clean:
	rm -rf ./build/
	rm -rf ./bin/

%_dir:
	$(NOECHO)mkdir -p $*

.PHONY: build clean
