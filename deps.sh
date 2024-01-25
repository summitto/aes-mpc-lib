#!/usr/bin/env bash

if [ "$(uname)" == "Darwin" ]; then
	brew list openssl || brew install openssl
 	brew list pkg-config || brew install pkg-config
 	brew list cmake || brew install cmake
else
    if command -v apt-get >/dev/null; then
        sudo apt-get install -y software-properties-common
        sudo apt-get update
        sudo apt-get install -y cmake git build-essential libssl-dev
    elif command -v yum >/dev/null; then
        sudo yum install -y python3 gcc make git cmake gcc-c++ openssl-devel
    else
        echo "System not supported yet!"
    fi
fi
