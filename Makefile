BUILDTYPE ?= Release

CLIPPER_REVISION=7484da1
PROTOZERO_REVISION=3868dfd6bd7ae393818ab1268fdd7bdcea2a9f0a
GYP_REVISION=3464008

all: vtile-encode

./node_modules/mapnik:
	npm install

./deps/gyp:
	git clone https://chromium.googlesource.com/external/gyp.git ./deps/gyp && cd ./deps/gyp && git checkout $(GYP_REVISION)

./deps/protozero:
	git clone https://github.com/mapbox/protozero.git ./deps/protozero && cd ./deps/protozero && git checkout $(PROTOZERO_REVISION)

./deps/clipper:
	git clone https://github.com/mapnik/clipper.git -b r496-mapnik ./deps/clipper && cd ./deps/clipper && git checkout $(CLIPPER_REVISION) && ./cpp/fix_members.sh

build/Makefile: ./deps/gyp ./deps/clipper ./deps/protozero gyp/build.gyp
	deps/gyp/gyp gyp/build.gyp --depth=. -Goutput_dir=. --generator-output=./build -f make

vtile-encode: build/Makefile Makefile vtile-encode.cpp
	@$(MAKE) -C build/ BUILDTYPE=$(BUILDTYPE) V=$(V)

test: vtile-encode
	./build/$(BUILDTYPE)/vtile-encode | protoc --decode_raw
	./build/$(BUILDTYPE)/vtile-encode | ./vtile2geojson.js


clean:
	rm -rf ./build
	rm -rf ./deps/protozero
	rm -rf ./deps/clipper

.PHONY: test
