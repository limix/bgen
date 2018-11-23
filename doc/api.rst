*********
Interface
*********

A BGEN file is associated with a :type:`bgen_file` variable, returned by
:func:`bgen_open`, and is required by many functions.
The user can query, for example, the number of samples contained in a BGEN file
by passing a :type:`bgen_file` variable to the :func:`bgen_nsamples`
function.
The user has to release resources by calling :func:`bgen_close` after its
use.

The function :func:`bgen_contain_sample` can be used to detect
whether the BGEN file contain sample identifications.
If it does, the function :func:`bgen_read_samples` will return an array
of sample identifications.
The allocated resources must be released by a subsequent call to
:func:`bgen_free_samples`.

The function :func:`bgen_read_metadata` reads the metadata of all variants
in the file (i.e., names, chromosomes, number of alleles, etc.).
It returns the read information as an array of type :type:`bgen_var`.
Let ``v`` be such an array.
The user can, for example, get the name of the i-th variant from
``v[i].id`` or its genetic position from ``v[i].position``.
After use, its resources have to be released by calling
:func:`bgen_free_variants_metadata`.

The function :func:`bgen_read_metadata` also returns another important
structure: the variants index, :type:`bgen_vi`.
It is used to locate the variants in a BGEN file.
Importantly, the variants index can be used even after the BGEN file has
been closed by a :func:`bgen_close` call.

To fetch a genotype information, the user has to first get a variant genotype
handler (:type:`bgen_vg`) by calling :func:`bgen_open_variant_genotype`.
The number of possible genotypes of a given variant, for example, can then be
found by a call to :func:`bgen_ncombs`.
The probabilities of each possible genotype can be found by a call to
:func:`bgen_read_variant_genotype`.
After use, the variant genotype handler has to be closed by
a :func:`bgen_close_variant_genotype` call.

Strings are represented by the :type:`bgen_str` type, which contains an
array of characters and its length.

File
^^^^

.. function:: struct bgen_file* bgen_open(const char* filepath)

    Create a bgen file handler.

    Remember to call :func:`bgen_close` to close the file and release
    resources after the interaction has finished.

    :param filepath: null-terminated file path to a bgen file.
    :return: bgen file handler.

.. function:: void bgen_close(struct bgen_file* bgen)

    Close a bgen file handler.

    :param bgen: bgen file handler.

.. function:: int bgen_nsamples(const struct bgen_file* bgen)

    Get the number of samples.

    :param bgen: bgen file handler.
    :return: number of samples.

.. function:: int bgen_nvariants(const struct bgen_file* bgen)

    Get the number of variants.

    :param bgen: bgen file handler.
    :return: number of variants.

.. function:: int bgen_contain_sample(const struct bgen_file* bgen)

    Check whether sample identifications are actually present.

    :param bgen: bgen file handler.
    :return: ``1`` for presence; ``0`` for absence.

.. function:: struct bgen_str* bgen_read_samples(struct bgen_file* bgen,\
    int verbose)

    Read sample identifications.

    Remember to call :func:`bgen_free_samples` on the returned array
    to release allocated resources after the interaction has finished.

    :param bgen: bgen file handler.
    :param verbose: ``1`` to show progress or ``0`` to disable output.
    :return: array of sample identifications.

.. function:: void bgen_free_samples(const struct bgen_file* bgen,\
    struct bgen_str* samples)

    Free memory associated with sample identifications.

    :param bgen: bgen file handler.
    :param samples: array of sample identifications.

Types
^^^^^

.. type:: bgen_str

    Represents a string.

    .. member:: int len

        String length.

    .. member:: char * str

        String itself. Not necessarily null-terminated.

.. type:: bgen_file

    BGEN file handler. Opaque struct.

.. type:: bgen_vi

    Variants index. Used internally to locate variants in a BGEN file.
    Opaque struct.

.. type:: bgen_vg

    Variant genotype handler. Used to query information about the genotype of
    a variant. Opaque struct.

.. type:: bgen_var

    Variant metadata.

    .. member:: bgen_str id

        Variant identifier.

    .. member:: bgen_str rsid

        Reference SNP cluster identifier.

    .. member:: bgen_str chrom

        Chromosome.

    .. member:: int position

        Genetic position.

    .. member:: int nalleles

        Number of alleles.

    .. member:: bgen_str * allele_ids

        Array of allele identifiers.

Variant genotype
^^^^^^^^^^^^^^^^

.. function:: struct bgen_vg* bgen_open_variant_genotype(struct bgen_vi* vi,\
    size_t index)

    Return a variant genotype handler.

    Remember to call :func:`bgen_close_variant_genotype` to the returned
    handler after the interaction has finished.

    .. seealso::

        - Use :func:`bgen_nvariants` to get the number of variants.
        - Use :func:`bgen_close_variant_genotype` to release resources.

    :param vi: variants index.
    :param index: array index of the requested variant.
    :return: variant genotype handler.

.. function:: void bgen_read_variant_genotype(struct bgen_vi* vi,\
    struct bgen_vg* vg, double* probs)

    Read the allele probabilities for a given variant.

    It is up to the user to pass an array of doubles of size given by
    a call to the function :func:`bgen_ncombs`.

    :param vi: variants index.
    :param vg: variant genotype handler.
    :param probs: allele probabilities.

.. function:: void bgen_close_variant_genotype(struct bgen_vi* vi,\
    struct bgen_vg* vg)

    Close the variant genotype reference.

    :param vi: variants index.
    :param vg: variant genotype handler.

.. function:: int bgen_nalleles(const struct bgen_vg* vg)

    Get the number of alleles.

    :param vg: variant genotype handler.
    :return: number of alleles.

.. function:: int bgen_missing(const struct bgen_vg *vg,\
    size_t index)

    Return ``1`` if variant is missing for the sample;
    ``0`` otherwise.

    .. note::

        Implemented in version 2.0.0.

    :param vg: variant genotype handler.
    :param index: sample index.
    :return: ``1`` for missing variant; ``0`` otherwise.

.. function:: int bgen_ploidy(const struct bgen_vg* vg,\
    size_t index)

    Get the ploidy for a given locus and sample index.

    .. note::

        Modified in version 2.0.0.

    :param vg: variant genotype handler.
    :param index: sample index.
    :return: ploidy.

.. function:: int bgen_min_ploidy(const struct bgen_vg *vg)

    Get the minimum ploidy of the variant.

    .. note::

        Implemented in version 2.0.0.

    :param vg: variant genotype handler.
    :return: minimum ploidy.

.. function:: int bgen_max_ploidy(const struct bgen_vg *vg)

    Get the maximum ploidy of the variant.

    .. note::

        Implemented in version 2.0.0.

    :param vg: variant genotype handler.
    :return: maximum ploidy.

.. function:: int bgen_ncombs(const struct bgen_vg *vg)

    Get the number of genotype combinations for a given variant.

    :param vg: variant genotype handler.
    :return: number of possible genotypes.

.. function:: int bgen_phased(const struct bgen_vg *vg)

    Return ``1`` for phased or ``0`` for unphased genotype.

    .. note::

        Implemented in version 2.0.0.

    :param vg: variant genotype handler.
    :return: ``1`` for phased genotype; ``0`` otherwise.


Variant metadata
^^^^^^^^^^^^^^^^

.. function:: struct bgen_var* bgen_read_metadata(struct bgen_file* bgen,\
    struct bgen_vi** vi, int verbose)

    Read variants metadata and index.

    Alternatively, the user can call
    :func:`bgen_load_variants_metadata` to read that information from a file
    created by calling :func:`bgen_store_variants_metadata`.

    Remember to call :func:`bgen_free_variants_metadata` on the returned array
    to release allocated resources after the interaction has finished.

    .. seealso::

        - Use :func:`bgen_free_variants_metadata` to release resources
          associated with variants metadata.
        - Use :func:`bgen_free_index` to release resources associated
          with the index.

    :param bgen: bgen file handler.
    :param vi: variants index.
    :param verbose: ``1`` to show progress or ``0`` to disable output.
    :return: variants information.

.. function:: void bgen_free_variants_metadata(const struct bgen_file* bgen,\
    struct bgen_var* variants)

    Free memory associated with variants metadata.

    :param bgen: bgen file handler.
    :param variants: variants information.

.. function:: void bgen_free_index(struct bgen_vi* vi)

    Free memory associated with variants index.

    :param vi: variants index.

.. function:: int bgen_max_nalleles(struct bgen_vi* vi)

    Get the maximum number of alleles across the entire file.

    :param vi: variants index.
    :return: maximum number of alleles.


Variants metadata file
^^^^^^^^^^^^^^^^^^^^^^

.. function:: int bgen_store_variants_metadata(const struct bgen_file* bgen,\
    struct bgen_var *variants, struct bgen_vi *vi, const char *filepath)

    Save variants metadata into a file for faster reloading.

    :param bgen: bgen file handler.
    :param variants: variants metadata.
    :param vi: variants index.
    :param filepath: null-terminated file path to the variants metadata.

.. function:: struct bgen_var *bgen_load_variants_metadata(\
    const struct bgen_file* bgen, const char *filepath,\
    struct bgen_vi** vi, int verbose)

    Load variants metadata from a file.

    :param bgen: bgen file handler.
    :param filepath: null-terminated file path to the variants metadata.
    :param vi: variants index.
    :param verbose: ``1`` to show progress or ``0`` to disable output.
    :return: variants metadata.


.. function:: int bgen_create_variants_metadata_file(const char *bgen_fp,\
    const char *vi_fp, int verbose)

    Create variants index and save it to a file.

    :param bgen_fp: null-terminated file path to a bgen file.
    :param vi_fp: null-terminated file path to the destination file.
    :param verbose: ``1`` to show progress or ``0`` to disable output.
    :return: ``0`` on success; ``1`` otherwise.
