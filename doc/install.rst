*******
Install
*******

A successful installation of this library will copy a dynamic and a static
libraries and a C header.
For example, in a GNU/Linux, an installation might consists in the following
files:

- /usr/local/lib/libbgen.so.VERSION
- /usr/local/lib/libbgen.so
- /usr/local/include/bgen.h
- /usr/local/include/bgen/*
- /usr/local/lib/libbgen_static.a

The easiest way of installing bgen is via `conda`_

.. code-block:: bash

  conda install -c conda-forge bgen

Alternatively, it can be compiled on GNU/Linux or MacOS by entering

.. code-block:: bash

  bash <(curl -fsSL https://raw.githubusercontent.com/limix/bgen/master/install)

or by entering


.. code-block:: powershell

  powershell -Command "(New-Object Net.WebClient).DownloadFile('https://raw.githubusercontent.com/limix/bgen/master/install.bat', 'install.bat')" && install.bat

on Windows.

The second option will work only if the Zstandard_ library has been properly
installed before-hand, as well as the zlib library (which is probably
already installed in your environment), and the athr_ library.

.. _conda: http://conda.pydata.org/docs/index.html
.. _Zstandard: http://facebook.github.io/zstd/
.. _athr: https://github.com/horta/almosthere
