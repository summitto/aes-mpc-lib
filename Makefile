.PHONY: clean-dep clean-client clean-go clean emp-ot emp-tool emp-agmpc install-deps client server

all: client server

clean-dep:
	$(MAKE) -C emp-tool/build clean
	$(MAKE) -C emp-ot/build clean
	$(MAKE) -C emp-agmpc/build clean

clean-client:
	rm -rf build

clean-go:
	rm -r swig-build
	rm libaesmpc.so

clean: clean-dep clean-client clean-go

# Submodule install targets below
emp-tool:
	$(MAKE) -C emp-tool/build

emp-ot: emp-tool
	$(MAKE) -C emp-ot/build

emp-agmpc: emp-ot
	$(MAKE) -C emp-agmpc/build

install-deps:
	sudo $(MAKE) -C emp-tool/build install && sudo $(MAKE) -C emp-ot/build install && sudo $(MAKE) -C emp-agmpc/build install

# Node.js client below
client: emp-agmpc
	npm run build

# Go server part below
ARCH := $(shell ./arch.sh)
WD := $(shell pwd)
INCLUDEDIRS := -I${OPENSSL_ROOT_DIR}/include -I${WD}/emp-tool -I${WD}/emp-ot -I${WD}/emp-agmpc
CXXFLAGS := -fpic -march=${ARCH} -maes -O3 -msse2 -std=c++11 -lm -fstack-clash-protection -fstack-protector-all -DNDEBUG -D_FORTIFY_SOURCE=2 ${INCLUDEDIRS}
LIBSEARCHDIRS := -L${OPENSSL_ROOT_DIR}/lib -L${WD}/emp-tool/build/
LIBRARIESFLAGS := -lpthread -ldl
LDFLAGS := -s ${LIBSEARCHDIRS} ${LIBRARIESFLAGS}

mpc.o: emp-agmpc
	mkdir -p swig-build && cd swig-build && \
	g++ -c ${CXXFLAGS} ../src/mpc.cpp

libaesmpc.so: mpc.o
	cd swig-build && \
	g++ -shared ${LDFLAGS} -o libaesmpc.so mpc.o && \
	mv libaesmpc.so ../

server: libaesmpc.so
	CGO_ENABLED=1 CGO_LDFLAGS="${LDFLAGS} -laesmpc" go install
