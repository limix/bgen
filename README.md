# bgen

[![Travis](https://travis-ci.com/limix/bgen.svg?branch=master)](https://travis-ci.com/limix/bgen)

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

A second installation option would be to download the latest source and to build it by yourself.
It can be as simple as:

```bash
curl -fsSL https://git.io/JerYI | GITHUB_USER=limix GITHUB_PROJECT=bgen bash
```

Under Windows, please, use Git Bash terminal to enter the above command.

## Documentation

Refer to [documentation](https://bgen.readthedocs.io/) for usage and API description.

## File specification

The original specification can be found at [http://www.well.ox.ac.uk/~gav/bgen_format/](http://www.well.ox.ac.uk/~gav/bgen_format/).
We have also created an alternative, more [user-friendly BGEN specification](bgen-file-format.pdf).

## Acknowledgments

- [Marc Jan Bonder](https://github.com/Bonder-MJ) for bug-reporting and improvement suggestions.
- [Yan Wong](https://github.com/hyanwong) for bug-reporting and improvement suggestions.
- [Carl Kadie](https://github.com/CarlKCarlK) for bug-reporting and improvement suggestions.

## Authors

- [Danilo Horta](https://github.com/horta)

## License

This project is licensed under the [MIT License](https://raw.githubusercontent.com/limix/bgen/master/LICENSE.md).
