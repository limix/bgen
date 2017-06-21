# bgen

[![Travis](https://img.shields.io/travis/limix/bgen.svg?style=flat-square)](https://travis-ci.org/limix/bgen)

A [BGEN file format](http://www.well.ox.ac.uk/~gav/bgen_format/) reader.

It fully supports all the BGEN format specifications: 1.1, 1.2, and 1.3.

## Requirements

It makes use of the [Zstandard](http://facebook.github.io/zstd/) library.
You don't need to install it by yourself if you choose to install bgen
via [conda](http://conda.pydata.org/docs/index.html).

## Install

You can install it via
[conda](http://conda.pydata.org/docs/index.html)

```bash
conda install -c conda-forge bgen
```

or by download one of the releases and building it

```bash
git clone https://github.com/limix/bgen.git
cd bgen
mkdir build
cd build
cmake .. -DZSTD_LIBRARIES=/path/to/libzstd.xxx -DZSTD_INCLUDE_DIRS=/path/to/include/dirs
make
make test
sudo make install
```

## Authors

* **Danilo Horta** - [https://github.com/horta](https://github.com/horta)

## License

This project is licensed under the MIT License - see the
[LICENSE](LICENSE) file for details
