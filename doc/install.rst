*******
Install
*******

You can install it via conda_:

.. code-block:: bash

  conda install -c conda-forge bgen

Alternatively, it can be installed via:

.. code-block:: bash

  curl -fsSL https://git.io/JerYI | GITHUB_USER=limix GITHUB_PROJECT=bgen bash

Under Windows, please, use Git Bash terminal to enter the above command.

The second option will work only if the zstandard_ library has been properly
installed before-hand, as well as the zlib_ library (which is probably
already installed in your system), and the athr_ library.

Please, report any issue by `opening an issue`_ on Github.

.. _conda: http://conda.pydata.org/docs/index.html
.. _zlib: https://github.com/horta/zlib.install
.. _Zstandard: https://github.com/horta/zstd.install
.. _athr: https://github.com/horta/almosthere
.. _opening an issue: https://github.com/limix/bgen/issues/new
