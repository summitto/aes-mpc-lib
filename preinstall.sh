#!/usr/bin/env bash

set -e

empDeps=(emp-tool emp-ot emp-agmpc)

# Initialize git submodules
if [[ -e ".git" ]]; then
  git submodule update --init --recursive
  submodulesDone=$?

  if [[ "$submodulesDone" != 0 ]]; then
    echo "Failed to initialize submodules, exiting"
    exit 1
  fi
else
  # If installing from NPM, this directory is not gonna be a git repo, so we must initialize it ourselves
  for lib in ${empDeps[@]}; do
    git clone --quiet --branch master --no-tags git@github.com:emp-toolkit/$lib.git $lib
  done

  git -C emp-tool checkout 00f755283f05a2bc7fe042893b2b9c8e99c5d140 > /dev/null && git -C emp-tool submodule update --init --recursive > /dev/null
  git -C emp-ot checkout eb0daf2a7a88c44b419f6d1276dc19e66f80776f > /dev/null && git -C emp-ot submodule update --init --recursive > /dev/null
  git -C emp-agmpc checkout 0add81ed517ac5b83d3a6576572b8daa0d236303 > /dev/null && git -C emp-agmpc submodule update --init --recursive > /dev/null
fi

for lib in ${empDeps[@]}; do
  # Patch submodules
  echo "Reverting local changes in $lib..."
  git -C $lib reset --hard > /dev/null
  echo "Patching $lib..."
  git apply --directory=$lib $lib.patch
  # Prepare make files in submodules
  cmake -S $lib -B $lib/build && sudo make -C $lib/build install
done
