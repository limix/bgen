#!/bin/bash
set -e -x

pushd .
rm -rf zstd-build || true
git clone https://github.com/facebook/zstd.git zstd-build
cd zstd-build
make

set +e
sudo &> /dev/null
err="$?"
if [[ "$err" == "1" ]]; then
    SUDO="sudo"
else
    SUDO=""
fi
set -e

eval "$SUDO" make install
if [[ $TRAVIS_OS_NAME == 'linux' ]]; then
    eval "$SUDO" ldconfig
fi
popd
rm -rf zstd-build || true
