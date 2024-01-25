# AES MPC lib for ProveThis

## Prerequisites

- CMake >3.13
- GCC >9

1. Install dependencies by running `./deps.sh`. This will not guarantee CMake and GCC versions so check them yourself.
2. Run `preinstall.sh`
3. Install JS dependencies `npm ci`

If you're on Mac, and have OpenSSL installed from brew, you may need to set `OPENSSL_ROOT_DIR` if any of the build commands fail to find <openssl/ec.h> for example.

```sh
export OPENSSL_ROOT_DIR=$(brew --prefix openssl)
```

or if your OpenSSL is not linked

```sh
export OPENSSL_ROOT_DIR=$(brew --prefix openssl@1.1)
```

## Building client for JS

1. Run `make client`. The package is ready to be used
2. (Optional) Run tests `npm run test` to test the binding. Run the server test as well.

## Building server for Go

1. Run `make server`.
2. (Optional) Run test by `go test`.

If you encounter errors like symbol is undefined while running tests or building the server part,
try adding `CGO_LDFLAGS="-lcrypto -lssl -ldl -lpthread -laesmpc"` before the `go build`/`go test` command.

On Mac M1 you may need to provide some extra environment variables - `GOARCH=arm64 CGO_ENABLED=1 CGO_LDFLAGS="-lcrypto -lssl -ldl -lpthread -laesmpc" go build/test`.

If Go cannot find `crypto` or `ssl` libraries on Mac, try adding `-L$(brew --prefix openssl)/lib` to `CGO_LDFLAGS` string. It may also be that OpenSSL is not linked,
then you may want to specify OpenSSL version like `brew --prefix openssl@1.1`.

### Generating SWIG wrapper (done once, or when the MPC lib changes API)

1. Run `swig -go -intgosize 64 -cgo -c++ aesmpc.i`
2. Add `// #cgo LDFLAGS: -L. -Lemp-tool/build -lcrypto -lssl -laesmpc -ldl -lpthread` right before `import "C"` line in `aesmpc.go`

