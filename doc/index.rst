====================
BGen's documentation
====================

:Date: |today|
:Version: |version|

A `BGEN file format`_ reader.

BGEN is a file format for storing large genetic datasets.
It supports both unphased genotypes and phased haplotype data with variable
ploidy and number of alleles.
It was designed to provides a compact data representation without sacrificing
variant access performance.

This is a low-memory footprint library that provides an easy-to-use C interface
to efficiently read BGEN files.
It fully supports all the BGEN format specifications: 1.2 and 1.3; as
well as their optional compressed formats.

.. toctree::
    :caption: Table of contents
    :name: mastertoc
    :maxdepth: 2

    install
    examples
    interface

.. _BGEN file format: http://www.well.ox.ac.uk/~gav/bgen_format/

*****************
Comments and bugs
*****************

You can get the source and open issues `on Github.`_

.. _on Github.: https://github.com/limix/bgen
