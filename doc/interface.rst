*********
Interface
*********

A BGEN file is associated with a :cpp:type:`bgen_file` variable, returned by
:cpp:func:`bgen_file_open`, and is required by many functions. The user can
query, for example, the number of samples contained in a BGEN file by passing
a :cpp:type:`bgen_file` variable to the :cpp:func:`bgen_file_nsamples`
function. The user has to release resources by calling
:cpp:func:`bgen_file_close` after its use.

The function :cpp:func:`bgen_file_contain_samples` can be used to detect
whether the BGEN file contain sample identifications. If it does, the function
:cpp:func:`bgen_file_read_samples` will return a struct of sample
identifications. The allocated resources must be released by a subsequent call
to :cpp:func:`bgen_samples_destroy`.

The function :cpp:func:`bgen_metafile_read_partition` reads the variants
metadata in the corresponding partition (i.e., names, chromosomes, number of
alleles, etc.). It returns the read information as an array of type
:cpp:type:`bgen_partition`. After use, its resources have to be released by
calling :cpp:func:`bgen_partition_destroy`.

To fetch a genotype information, the user has to first get a variant genotype
handler (:cpp:type:`bgen_genotype`) by calling
:cpp:func:`bgen_file_open_genotype`. The number of possible genotypes of
a given variant, for example, can then be found by a call to
:cpp:func:`bgen_genotype_ncombs`. The probabilities of each possible genotype
can be found by a call to :cpp:func:`bgen_genotype_read`. After use, the
variant genotype handler has to be closed by a :cpp:func:`bgen_genotype_close`
call.

Strings are represented by the :cpp:type:`bgen_string` type, which contains an
array of characters and its length.

File
^^^^

.. doxygenfunction:: bgen_file_open
.. doxygenfunction:: bgen_file_close
.. doxygenfunction:: bgen_file_nsamples
.. doxygenfunction:: bgen_file_nvariants
.. doxygenfunction:: bgen_file_contain_samples
.. doxygenfunction:: bgen_file_read_samples
.. doxygenfunction:: bgen_file_open_genotype
.. doxygenstruct:: bgen_file

Genotype
^^^^^^^^

.. doxygenfunction:: bgen_genotype_close
.. doxygenfunction:: bgen_genotype_read
.. doxygenfunction:: bgen_genotype_read64
.. doxygenfunction:: bgen_genotype_read32
.. doxygenfunction:: bgen_genotype_nalleles
.. doxygenfunction:: bgen_genotype_missing
.. doxygenfunction:: bgen_genotype_ploidy
.. doxygenfunction:: bgen_genotype_min_ploidy
.. doxygenfunction:: bgen_genotype_max_ploidy
.. doxygenfunction:: bgen_genotype_ncombs
.. doxygenfunction:: bgen_genotype_phased
.. doxygenstruct:: bgen_genotype

Metafile
^^^^^^^^

.. doxygenfunction:: bgen_metafile_create
.. doxygenfunction:: bgen_metafile_open
.. doxygenfunction:: bgen_metafile_npartitions
.. doxygenfunction:: bgen_metafile_nvariants
.. doxygenfunction:: bgen_metafile_read_partition
.. doxygenfunction:: bgen_metafile_close
.. doxygenstruct:: bgen_metafile

Partition
^^^^^^^^^

.. doxygenfunction:: bgen_partition_destroy
.. doxygenfunction:: bgen_partition_get
.. doxygenfunction:: bgen_partition_nvariants
.. doxygenstruct:: bgen_partition

Samples
^^^^^^^

.. doxygenfunction:: bgen_samples_destroy
.. doxygenfunction:: bgen_samples_get
.. doxygenstruct:: bgen_samples

String
^^^^^^

.. doxygenfunction:: bgen_string_create
.. doxygenfunction:: bgen_string_destroy
.. doxygenfunction:: bgen_string_data
.. doxygenfunction:: bgen_string_length
.. doxygenfunction:: bgen_string_equal
.. doxygenfunction:: BGEN_STRING
.. doxygenstruct:: bgen_string

Variant
^^^^^^^

.. doxygenstruct:: bgen_variant
   :members:

.. |bgen format specification| raw:: html

   <a href="https://www.well.ox.ac.uk/~gav/bgen_format/" target="_blank">bgen format specification⧉</a>
