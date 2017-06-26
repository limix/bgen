# bgen

[![Travis](https://img.shields.io/travis/limix/bgen.svg?style=flat-square)](https://travis-ci.org/limix/bgen)
[![Appveyor](https://ci.appveyor.com/api/projects/status/9ygi9jksbgikb87y/branch/master?svg=true)](https://ci.appveyor.com/project/Horta/bgen)
[![Documentation Status](https://readthedocs.org/projects/bgen/badge/?style=flat-square&version=latest)](https://bgen.readthedocs.io/)

A [BGEN file format](http://www.well.ox.ac.uk/~gav/bgen_format/) reader.

It fully supports all the BGEN format specifications: 1.1, 1.2, and 1.3.

## Requirements

It makes use of the [Zstandard](http://facebook.github.io/zstd/) library.
You don't need to install it by yourself if you choose to install bgen
via [conda](http://conda.pydata.org/docs/index.html) but you do need it
installed before-hand if you choose to build bgen library by yourself.

## Install

The recommended way is to install it via
[conda](http://conda.pydata.org/docs/index.html)

```bash
conda install -c conda-forge bgen
```

A second option would download the latest source and building
it by yourself.
On Linux or macOS systems it can be as simple as
```bash
wget https://github.com/limix/bgen/archive/0.1.12.tar.gz
tar xzf 0.1.12.tar.gz
cd bgen-0.1.12
mkdir build
cd build
cmake ..
make
make test
sudo make install
```
assuming that you have both zlib (often the case) and Zstandard (usually not
the case) libraries installed and that cmake managed to find them without
any hint.
If you do have those libraries installed but cmake did not manage to find
them, you can specify their location to cmake as
```bash
cmake .. -DZLIB_ROOT="/path/to/zlib/dir" -DZSTD_LIBRARYDIR="/path/to/zstd/lib/dir"
-DZSTD_INCLUDEDIR="/path/to/zstd/header/dir"
```

On Windows systems you might want to have a look at the
[nmake](https://msdn.microsoft.com/en-us/library/dd9y37ha.aspx) command
as a replacement for ``make``.
Assuming you are at folder ``C:\projects\bgen`` and that you have installed
zlib and Zstandard libraries into ``C:\projects\bgen\deps``, you might want
to try
```
mkdir build
cd build
cmake .. -G "NMake Makefiles" -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=TRUE -DZLIB_ROOT="C:\projects\bgen\deps\zlib" -DZSTD_LIBRARYDIR="C:\projects\bgen\deps\zstd\lib" -DZSTD_INCLUDEDIR="C:\projects\bgen\deps\zstd\include"
nmake
nmake test
nmake install
```

## Documentation

Refer to [documentation](https://bgen.readthedocs.io/) for usage and API
description.

## Authors

* **Danilo Horta** - [https://github.com/horta](https://github.com/horta)

## License

This project is licensed under the MIT License - see the
[LICENSE](LICENSE) file for details
