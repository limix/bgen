# bgen

[![Travis](https://img.shields.io/travis/limix/bgen.svg?style=flat-square&label=linux%20%2F%20macos%20build)](https://travis-ci.org/limix/bgen) [![AppVeyor](https://img.shields.io/appveyor/ci/Horta/bgen.svg?style=flat-square&label=windows%20build)](https://ci.appveyor.com/project/Horta/bgen)

A [BGEN file format](http://www.well.ox.ac.uk/~gav/bgen_format/) reader.

It fully supports the BGEN format specifications 1.2 and 1.3.

## Requirements

It makes use of the [Zstandard library](http://facebook.github.io/zstd/). You don't need to install it by yourself if you choose to install bgen via [conda](http://conda.pydata.org/docs/index.html) but you do need it installed before-hand if you choose to build bgen library by yourself. It also makes use of the [almosthere library](https://github.com/horta/almosthere) for showing progress while performing time-consuming tasks:

- [zstd install](https://github.com/horta/zstd.install)
- [zlib install](https://github.com/horta/zlib.install)
- [almosthere install](https://github.com/horta/almosthere)

## Install

You can install it via `conda`:

```bash
conda install -c conda-forge bgen
```

A second installation option would be to download the latest source and to build it by yourself. On Linux or macOS systems it can be as simple as:

```bash
bash <(curl -fsSL https://raw.githubusercontent.com/limix/bgen/master/install)
```

Similarly, on Windows you can install by entering in the terminal:

```powershell
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://raw.githubusercontent.com/limix/bgen/master/install.bat', 'install.bat')" && install.bat
```

assuming that you have both zlib (often the case) and Zstandard (usually not the case) libraries installed and that [cmake](https://cmake.org/) managed to find them without any hint. If you do have those libraries installed but `cmake` did not manage to find them, you can specify their location to `cmake` as:

    cmake .. -DZLIB_ROOT="/path/to/zlib/dir" \
             -DZSTD_LIBRARYDIR="/path/to/zstd/lib/dir" \
             -DZSTD_INCLUDEDIR="/path/to/zstd/header/dir"

## Documentation

Refer to [documentation](https://bgen.readthedocs.io/) for usage and API description.

## File specification

The original specification can be found at [http://www.well.ox.ac.uk/~gav/bgen_format/](http://www.well.ox.ac.uk/~gav/bgen_format/).
We have also created an alternative, more [user-friendly BGEN specification](bgen-file-format.pdf).

## Development

Create a `build` folder, configure, build and test it.
The option `-DCMAKE_EXPORT_COMPILE_COMMANDS=ON` will create the
`compile_commands.json` file, which is often used by a language server
protocol to provide context to your editor.

```bash
mkdir build
cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug ..
make
make test
```

[Valgrind](http://www.valgrind.org/) can be used to detect memory leak and wrong memory access.
Docker can be used to test it on a Linux distribution.
From the project folder, enter

```bash
docker run --cap-add=SYS_PTRACE --security-opt seccomp=unconfined \
    -v $(pwd):/$(basename $(pwd)) -w /$(basename $(pwd)) -it ubuntu /bin/bash
```

The above options make sure that gdb can be used in the docker container.
A couple of dependencies have to be installed first:

```bash
apt-get update && apt-get upgrade --yes
apt-get install build-essential curl zlib1g-dev cmake gdb
bash <(curl -fsSL https://raw.githubusercontent.com/horta/almosthere/master/install)
bash <(curl -fsSL https://raw.githubusercontent.com/horta/zstd.install/master/install)
```

After everything has been compiled, you can enter the `test` folder and fire valgrind:

```bash
cd test
valgrind --leak-check=full --show-leak-kinds=all ./index
```

## Acknowledgments

- [Marc Jan Bonder](https://github.com/Bonder-MJ) for bug-reporting and improvement suggestions.
- [Yan Wong](https://github.com/hyanwong) for bug-reporting and improvement suggestions.

## Authors

- [Danilo Horta](https://github.com/horta)

## License

This project is licensed under the [MIT License](https://raw.githubusercontent.com/limix/bgen/master/LICENSE.md).
