*******
Install
*******

You can install it via `conda`_ (the recommended way)

.. code-block:: bash

  conda install -c conda-forge bgen

or by cloning this repository and building it

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
already installed in your environment), and the ProgressBar_ library.

.. _conda: http://conda.pydata.org/docs/index.html
.. _Zstandard: http://facebook.github.io/zstd/
.. _ProgressBar: https://github.com/limix/progressbar
