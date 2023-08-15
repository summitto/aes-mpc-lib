#!/usr/bin/env bash

if [[ "$(uname -m)" == "arm64" ]]
then
  echo "armv8-a+simd+crypto+crc"
else
  echo "native"
fi
