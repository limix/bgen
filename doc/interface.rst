*********
Interface
*********

A BGEN file is associated with a :cpp:type:`bgen_file` variable, returned by
:cpp:func:`bgen_open`, and is required by many functions.
The user can query, for example, the number of samples contained in a BGEN file
by passing a :cpp:type:`bgen_file` variable to the :cpp:func:`bgen_nsamples`
function.
The user has to release resources by calling :cpp:func:`bgen_close` after its
use.

The function :cpp:func:`bgen_contain_samples` can be used to detect
whether the BGEN file contain sample identifications.
If it does, the function :cpp:func:`bgen_read_samples` will return an array
of sample identifications.
The allocated resources must be released by a subsequent call to
:cpp:func:`bgen_free_samples`.

The function :cpp:func:`bgen_read_partition` reads the variants metadata in the
corresponding partition (i.e., names, chromosomes, number of alleles, etc.).
It returns the read information as an array of type :cpp:type:`bgen_vm`.
Let ``v`` be such an array.
The user can, for example, get the name of the i-th variant from
``v[i].id`` or its genetic position from ``v[i].position``.
After use, its resources have to be released by calling
:cpp:func:`bgen_free_partition`.

The function :cpp:func:`bgen_read_partition` also returns another important
information: the variant address.
It is used to locate the variants in a BGEN file.

To fetch a genotype information, the user has to first get a variant genotype
handler (:cpp:type:`bgen_vg`) by calling :cpp:func:`bgen_open_genotype`.
The number of possible genotypes of a given variant, for example, can then be
found by a call to :cpp:func:`bgen_ncombs`.
The probabilities of each possible genotype can be found by a call to
:cpp:func:`bgen_read_genotype`.
After use, the variant genotype handler has to be closed by
a :cpp:func:`bgen_close_genotype` call.

Strings are represented by the :cpp:type:`bgen_str` type, which contains an
array of characters and its length.


File
^^^^

.. doxygenfunction:: bgen_open
.. doxygenfunction:: bgen_close
.. doxygenfunction:: bgen_nsamples
.. doxygenfunction:: bgen_nvariants
.. doxygenfunction:: bgen_contain_samples
.. doxygenfunction:: bgen_read_samples
.. doxygenfunction:: bgen_free_samples

Types
^^^^^

.. doxygenstruct:: bgen_str
   :members:
.. doxygenstruct:: bgen_file
.. doxygenstruct:: bgen_vg
.. doxygenstruct:: bgen_vm
   :members:


Genotype
^^^^^^^^

.. doxygenfunction:: bgen_open_genotype
.. doxygenfunction:: bgen_close_genotype
.. doxygenfunction:: bgen_read_genotype
.. doxygenfunction:: bgen_nalleles
.. doxygenfunction:: bgen_missing
.. doxygenfunction:: bgen_ploidy
.. doxygenfunction:: bgen_min_ploidy
.. doxygenfunction:: bgen_max_ploidy
.. doxygenfunction:: bgen_ncombs
.. doxygenfunction:: bgen_phased


Metadata
^^^^^^^^

.. doxygenfunction:: bgen_create_metafile
.. doxygenfunction:: bgen_open_metafile
.. doxygenfunction:: bgen_metafile_npartitions
.. doxygenfunction:: bgen_metafile_nvariants
.. doxygenfunction:: bgen_read_partition
.. doxygenfunction:: bgen_free_partition
.. doxygenfunction:: bgen_close_metafile


Deprecated
^^^^^^^^^^

.. doxygenstruct:: bgen_var
   :members:
.. doxygenstruct:: bgen_vi
   :members:
.. doxygenstruct:: bgen_string
   :members:
.. doxygenfunction:: bgen_open_variant_genotype
.. doxygenfunction:: bgen_read_variant_genotype
.. doxygenfunction:: bgen_close_variant_genotype
.. doxygenfunction:: bgen_sample_ids_presence
.. doxygenfunction:: bgen_read_variants_metadata
.. doxygenfunction:: bgen_free_variants_metadata
.. doxygenfunction:: bgen_free_index
.. doxygenfunction:: bgen_store_variants_metadata
.. doxygenfunction:: bgen_load_variants_metadata
.. doxygenfunction:: bgen_create_variants_metadata_file
.. doxygenfunction:: bgen_max_nalleles

.. |bgen format specification| raw:: html

   <a href="https://www.well.ox.ac.uk/~gav/bgen_format/" target="_blank">bgen format specification⧉</a>
