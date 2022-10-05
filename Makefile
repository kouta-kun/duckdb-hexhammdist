.PHONY: all clean format debug release pull update

all: release

OSX_BUILD_UNIVERSAL_FLAG=
ifeq (${OSX_BUILD_UNIVERSAL}, 1)
	OSX_BUILD_UNIVERSAL_FLAG=-DOSX_BUILD_UNIVERSAL=1
endif
BUILD_FLAGS=-DEXTENSION_STATIC_BUILD=1 -DBUILD_TPCH_EXTENSION=1 ${OSX_BUILD_UNIVERSAL_FLAG}


pull:
	git submodule init
	git submodule update

clean:
	rm -rf build

debug: pull
	mkdir -p build/debug && \
	cd build/debug && \
	cmake -DCMAKE_BUILD_TYPE=Debug ${BUILD_FLAGS} ../../duckdb/CMakeLists.txt -DEXTERNAL_EXTENSION_DIRECTORY=../../duckdb-hexhammdist -B. && \
	make -j hexhammdist_extension_loadable_extension

release: pull 
	mkdir -p build/release && \
	cd build/release && \
	cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ${BUILD_FLAGS} ../../duckdb/CMakeLists.txt -DEXTERNAL_EXTENSION_DIRECTORY=../../duckdb-hexhammdist -B. && \
	make -j hexhammdist_extension_loadable_extension

fulltree: pull release
	cd build/release && \
	make -j


test: fulltree
	./build/release/test/unittest --test-dir . "[hexhamm]"

format:
	cp duckdb/.clang-format .
	clang-format --sort-includes=0 -style=file -i hexhammdist_extension.cpp
	cmake-format -i CMakeLists.txt
	rm .clang-format

update:
	git submodule update --remote --merge
