# bgen

[![Travis](https://img.shields.io/travis/limix/bgen.svg?style=flat-square&label=linux%20%2F%20macos%20build)](https://travis-ci.org/limix/bgen) [![AppVeyor](https://img.shields.io/appveyor/ci/limix/bgen.svg?style=flat-square&label=windows%20build)](https://ci.appveyor.com/project/limix/bgen)

A [BGEN file format](http://www.well.ox.ac.uk/~gav/bgen_format/) reader.

It fully supports all the BGEN format specifications: 1.2 and 1.3.

## Requirements

It makes use of the [Zstandard library](http://facebook.github.io/zstd/). You don't need to install it by yourself if you choose to install bgen via [conda](http://conda.pydata.org/docs/index.html) but you do need it installed before-hand if you choose to build bgen library by yourself. It also makes use of the [progressbar library](https://github.com/limix/progressbar) for showing progress while performing time-consuming tasks.

## Install

You can install it via `conda`:

    conda install -c conda-forge bgen

A second installation option would be to download the latest source and to build it by yourself. On Linux or macOS systems it can be as simple as:

    bash <(curl -fsSL https://raw.githubusercontent.com/limix/bgen/master/install)

assuming that you have both zlib (often the case) and Zstandard (usually not the case) libraries installed and that [cmake](https://cmake.org/) managed to find them without any hint. If you do have those libraries installed but `cmake` did not manage to find them, you can specify their location to `cmake` as:

    cmake .. -DZLIB_ROOT="/path/to/zlib/dir" \
             -DZSTD_LIBRARYDIR="/path/to/zstd/lib/dir" \
             -DZSTD_INCLUDEDIR="/path/to/zstd/header/dir"

On Windows systems you might want to have a look at the [nmake](https://msdn.microsoft.com/en-us/library/dd9y37ha.aspx) command as a replacement for `make`. Assuming you are at folder `C:\projects\bgen` and that you have installed zlib and Zstandard libraries into `C:\projects\bgen\deps`, you might want to try:

    mkdir build && cd build
    cmake .. -G "NMake Makefiles"
    cmake --build .
    ctest
    nmake install

## Documentation

Refer to [documentation](https://bgen.readthedocs.io/) for usage and API description.

## Authors

  - [Danilo Horta](https://github.com/horta)

## License

This project is licensed under the MIT License - see the [license file](https://raw.githubusercontent.com/limix/bgen/master/LICENSE.txt) for details.
