
bgen
====

|Build-Status| |Win-Build-Status| |Codacy-Grade| |Doc-Status|

A `BGEN file format`_ reader.

It fully supports all the BGEN format specifications: 1.1, 1.2, and 1.3.

Requirements
------------

It makes use of the [Zstandard](http://facebook.github.io/zstd/) library.
You don't need to install it by yourself if you choose to install bgen
via [conda](http://conda.pydata.org/docs/index.html) but you do need it
installed before-hand if you choose to build bgen library by yourself.

Install
-------

You can install it via conda_

.. code:: bash

    conda install -c conda-forge bgen

A second installation option would be to download the latest source and to
build it by yourself.
On Linux or macOS systems it can be as simple as

.. code:: bash

    wget https://github.com/limix/bgen/archive/0.1.14.tar.gz
    tar xzf 0.1.14.tar.gz
    cd bgen-0.1.14
    mkdir build
    cd build
    cmake ..
    make
    make test
    sudo make install

assuming that you have both zlib (often the case) and Zstandard (usually not
the case) libraries installed and that ``cmake`` managed to find them without
any hint.
If you do have those libraries installed but ``cmake`` did not manage to find
them, you can specify their location to ``cmake`` as

.. code:: bash

    cmake .. -DZLIB_ROOT="/path/to/zlib/dir" \
             -DZSTD_LIBRARYDIR="/path/to/zstd/lib/dir" \
             -DZSTD_INCLUDEDIR="/path/to/zstd/header/dir"

On Windows systems you might want to have a look at the nmake_ command
as a replacement for ``make``.
Assuming you are at folder ``C:\projects\bgen`` and that you have installed
zlib and Zstandard libraries into ``C:\projects\bgen\deps``, you might want
to try

.. code:: dos

    mkdir build
    cd build
    cmake .. -G "NMake Makefiles" ^
         -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=TRUE ^
         -DZLIB_ROOT="C:\projects\bgen\deps\zlib" ^
         -DZSTD_LIBRARYDIR="C:\projects\bgen\deps\zstd\lib" ^
         -DZSTD_INCLUDEDIR="C:\projects\bgen\deps\zstd\include"
    nmake
    nmake test
    nmake install

Documentation
-------------

Refer to documentation_ for usage and API description.

Authors
-------

* `Danilo Horta`_

License
-------

This project is licensed under the MIT License - see the `License file`_ file
for details.


.. |Build-Status| image:: https://travis-ci.org/limix/bgen.svg?branch=master
    :target: https://travis-ci.org/limix/bgen

.. |Win-Build-Status| image:: https://ci.appveyor.com/api/projects/status/kb4b4rcsm4t60bg5/branch/master?svg=true
    :target: https://ci.appveyor.com/project/Horta/bgen/branch/master

.. |Codacy-Grade| image:: https://api.codacy.com/project/badge/Grade/689b555393364226863c3a237f801650
    :target: https://www.codacy.com/app/danilo.horta/bgen?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=limix/bgen&amp;utm_campaign=Badge_Grade

.. |Doc-Status| image:: https://readthedocs.org/projects/bgen/badge/?style=flat-square&version=stable
    :target: https://bgen.readthedocs.io/

.. _conda: http://conda.pydata.org/docs/index.html

.. _License file: https://raw.githubusercontent.com/limix/bgen/master/LICENSE.txt

.. _Danilo Horta: https://github.com/horta

.. _documentation: http://bgen.readthedocs.io/

.. _BGEN file format: http://www.well.ox.ac.uk/~gav/bgen_format/

.. _nmake: https://msdn.microsoft.com/en-us/library/dd9y37ha.aspx
