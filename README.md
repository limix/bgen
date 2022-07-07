# bgen

A [BGEN file format](http://www.well.ox.ac.uk/~gav/bgen_format/) reader.

It fully supports the BGEN format specifications 1.2 and 1.3.

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

## Development

Make sure you have the dependencies installed.
From terminal, clone the repository and checkout the develop branch:

```bash
git clone https://github.com/limix/bgen.git
cd bgen
```

Create a build folder and compile from there:

```bash
mkdir build
cd build
# Configure and generate scripts that will build it
cmake -DCMAKE_BUILD_TYPE=Release ..
# Compile it
cmake --build . --config Release
# Test it
ctest --output-on-failure -C Release
```

## Acknowledgments

- [Marc Jan Bonder](https://github.com/Bonder-MJ) for bug-reporting and improvement suggestions.
- [Yan Wong](https://github.com/hyanwong) for bug-reporting and improvement suggestions.
- [Carl Kadie](https://github.com/CarlKCarlK) for bug-reporting and improvement suggestions.

## Authors

- [Danilo Horta](https://github.com/horta)

## License

This project is licensed under the [MIT License](https://raw.githubusercontent.com/limix/bgen/main/LICENSE.md).
