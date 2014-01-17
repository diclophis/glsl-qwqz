# OSX Makefile, needs to know where the main src root is

TOTAL_ALLOWED_RAM=65536

build=build
impl=$(IMPL)

CXXFLAGS=\
-O3 \
-I../../src \
-I../../src/contrib \
-I../../$(impl)-src \
-DDESKTOP \
-DNDEBUG

CFLAGS=$(CXXFLAGS)

objects   = $(patsubst %,build/%, $(patsubst %.c,%.bc, $(wildcard *.c)))
objects   += $(patsubst ../../src/%,build/%, $(patsubst %.c,%.bc, $(wildcard ../../src/contrib/zlib/*.c)))
objects   += $(patsubst ../../src/%,build/%, $(patsubst %.c,%.bc, $(wildcard ../../src/contrib/chipmunk/*.c)))
objects   += $(patsubst ../../src/%,build/%, $(patsubst %.c,%.bc, $(wildcard ../../src/contrib/spine/*.c)))
objects   += $(patsubst ../../src/%,build/%, $(patsubst %.c,%.bc, $(wildcard ../../src/*.c)))
objects   += $(patsubst ../../$(impl)-src/%,build/%, $(patsubst %.c,%.bc, $(wildcard ../../$(impl)-src/*.c)))

HASH=qwqz-$(shell md5sum build/raptor_island.js | cut -d" " -f 1)
hash_dir=~/emscripten.risingcode.com/public/$(HASH)

raptor_island: $(build) $(objects)
	$(CXX) -O3 $(objects) -o $(build)/raptor_island.bc
	$(CXX) -O3 -s ASM_JS=1 -s VERBOSE=0 -s GL_DEBUG=0 -s PRECISE_I64_MATH=1 -s TOTAL_MEMORY=$(TOTAL_ALLOWED_RAM) -s TOTAL_STACK=$(TOTAL_ALLOWED_RAM) -s FAST_MEMORY=$(TOTAL_ALLOWED_RAM) -s EXPORTED_FUNCTIONS="['_main', '_malloc', '_free', '_start_game', '_command']" $(build)/raptor_island.bc -o $(build)/raptor_island.js --js-library library_window.js

$(build)/%.bc: %.c
	$(CC) $(CXXFLAGS) -c $< -o $@

$(build)/%.bc: ../../$(impl)-src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(build)/%.bc: ../../src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(build):
	mkdir -p $(build)
	mkdir -p $(build)/contrib/zlib
	mkdir -p $(build)/contrib/chipmunk
	mkdir -p $(build)/contrib/spine

clean:
	touch /tmp/emscripten_temp && rm -R /tmp/emscripten_temp
	touch $(build) && rm -Rf $(build)

$(hash_dir):
	mkdir $(hash_dir)

install: $(hash_dir) build/raptor_island.js
	ln -nsf $(hash_dir) ~/emscripten.risingcode.com/public/latest-qwqz
	ruby cache_manifest.rb ../../assets > ~/emscripten.risingcode.com/public/latest-qwqz/index.appcache
	cp -R ../../assets ~/emscripten.risingcode.com/public/latest-qwqz
	touch ~/emscripten.risingcode.com/public/latest-qwqz/favicon.ico
	install -C index.html ~/emscripten.risingcode.com/public/latest-qwqz
	install -C $(build)/raptor_island.js ~/emscripten.risingcode.com/public/latest-qwqz
