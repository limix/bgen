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
- /usr/local/lib/libbgen_static.a

The recommended way of installing bgen is via `conda`_

.. code-block:: bash

  conda install -c conda-forge bgen

Alternatively, you can clone its repository and build it

.. code-block:: bash

  git clone https://github.com/limix/bgen.git
  cd bgen
  mkdir build
  cd build
  cmake ..
  make
  sudo make install

The second option will work only if the Zstandard_ library has been properly
installed before-hand, as well as the zlib library (which is probably
already installed in your environment), and the athr_ library.

.. _conda: http://conda.pydata.org/docs/index.html
.. _Zstandard: http://facebook.github.io/zstd/
.. _athr: https://github.com/horta/almosthere
