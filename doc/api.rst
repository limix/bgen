*********
Interface
*********

A BGEN file is associated with a :c:type:`bgen_file` variable, returned by
:c:func:`bgen_open`, and is required by many functions.
The user can query, for example, the number of samples contained in a BGEN file
by passing a :c:type:`bgen_file` variable to the :c:func:`bgen_nsamples`
function.
The user has to release resources by calling :c:func:`bgen_close` after its
use.

The function :c:func:`bgen_sample_ids_presence` can be used to detect
whether the BGEN file contain sample identifications.
If it does, the function :c:func:`bgen_read_samples` will return an array
of sample identifications.
The allocated resources must be released by a subsequent call to
:c:func:`bgen_free_samples`.

The function :c:func:`bgen_read_variants_metadata` reads the metadata of all variants
in the file (i.e., names, chromosomes, number of alleles, etc.).
It returns the read information as an array of type :c:type:`bgen_var`.
Let ``v`` be such an array.
The user can, for example, get the name of the i-th variant from
``v[i].id`` or its genetic position from ``v[i].position``.
After use, its resources have to be released by calling
:c:func:`bgen_free_variants_metadata`.

The function :c:func:`bgen_read_variants_metadata` also returns another important
structure: the variants index, :c:type:`bgen_vi`.
It is used to locate the variants in a BGEN file.
Importantly, the variants index can be used even after the BGEN file has
been closed by a :c:func:`bgen_close` call.

To fetch a genotype information, the user has to first get a variant genotype
handler (:c:type:`bgen_vg`) by calling :c:func:`bgen_open_variant_genotype`.
The number of possible genotypes of a given variant, for example, can then be
found by a call to :c:func:`bgen_ncombs`.
The probabilities of each possible genotype can be found by a call to
:c:func:`bgen_read_variant_genotype`.
After use, the variant genotype handler has to be closed by
a :c:func:`bgen_close_variant_genotype` call.

Strings are represented by the :c:type:`bgen_string` type, which contains an
array of characters and its length.

File
^^^^

.. c:function:: struct bgen_file* bgen_open(const char* filepath)

    Create a bgen file handler.

    Remember to call :c:func:`bgen_close` to close the file and release
    resources after the interaction has finished.

    :param filepath: null-terminated file path to a bgen file.
    :return: bgen file handler.

.. c:function:: void bgen_close(struct bgen_file* bgen)

    Close a bgen file handler.

    :param bgen: bgen file handler.

.. c:function:: int bgen_nsamples(const struct bgen_file* bgen)

    Get the number of samples.

    :param bgen: bgen file handler.
    :return: number of samples.

.. c:function:: int bgen_nvariants(const struct bgen_file* bgen)

    Get the number of variants.

    :param bgen: bgen file handler.
    :return: number of variants.

.. c:function:: int bgen_sample_ids_presence(const struct bgen_file* bgen)

    Check whether sample identifications are actually present.

    :param bgen: bgen file handler.
    :return: ``1`` for presence; ``0`` for absence.

.. c:function:: bgen_string* bgen_read_samples(struct bgen_file* bgen,\
    int verbose)

    Read sample identifications.

    Remember to call :c:func:`bgen_free_samples` on the returned array
    to release allocated resources after the interaction has finished.

    :param bgen: bgen file handler.
    :param verbose: ``1`` to show progress or ``0`` to disable output.
    :return: array of sample identifications.

.. c:function:: void bgen_free_samples(const struct bgen_file* bgen,\
    bgen_string* samples)

    Free memory associated with sample identifications.

    :param bgen: bgen file handler.
    :param samples: array of sample identifications.

Types
^^^^^

.. c:type:: bgen_string

    Represents a string.

    .. c:member:: int len

        String length.

    .. c:member:: char * str

        String itself. Not necessarily null-terminated.

.. c:type:: bgen_file

    BGEN file handler.

.. c:type:: bgen_vi

    Variants index. Used internally to locate variants in a BGEN file.

.. c:type:: bgen_vg

    Variant genotype handler. Used to query information about the genotype of
    a variant.

.. c:type:: bgen_var

    Variant metadata.

    .. c:member:: bgen_string id

        Variant identifier.

    .. c:member:: bgen_string rsid

        Reference SNP cluster identifier.

    .. c:member:: bgen_string chrom

        Chromosome.

    .. c:member:: int position

        Genetic position.

    .. c:member:: int nalleles

        Number of alleles.

    .. c:member:: bgen_string * allele_ids

        Array of allele identifiers.

Variant genotype
^^^^^^^^^^^^^^^^

.. c:function:: struct bgen_vg* bgen_open_variant_genotype(struct bgen_vi* vi,\
    size_t index)

    Return a variant genotype handler.

    Remember to call :c:func:`bgen_close_variant_genotype` to the returned
    handler after the interaction has finished.

    .. seealso::

        Use :c:func:`bgen_nvariants` to get the number of variants.


    :param vi: variants index.
    :param index: array index of the requested variant.
    :return: variant genotype handler.

.. c:function:: void bgen_read_variant_genotype(struct bgen_vi* vi,\
    struct bgen_vg* vg, double* probs)

    Read the allele probabilities for a given variant.

    It is up to the user to pass an array of doubles of size given by
    a call to the function :c:func:`bgen_ncombs`.

    :param vi: variants index.
    :param vg: variant genotype handler.
    :param probs: allele probabilities.

.. c:function:: void bgen_close_variant_genotype(struct bgen_vi* vi,\
    struct bgen_vg* vg)

    Close the variant genotype reference.

    :param vi: variants index.
    :param vg: variant genotype handler.

.. c:function:: int bgen_nalleles(const struct bgen_vg* vg)

    Get the number of alleles.

    :param vg: variant genotype handler.
    :return: number of alleles.

.. c:function:: int bgen_ploidy(const struct bgen_vg* vg)

    Get the ploidy.

    :param vg: variant genotype handler.
    :return: ploidy.

.. c:function:: int bgen_ncombs(const struct bgen_vg *vg)

    Get the number of genotype combinations for a given variant.

    :param vg: variant genotype handler.
    :return: number of possible genotypes.



Variant metadata
^^^^^^^^^^^^^^^^

.. c:function:: struct bgen_var* bgen_read_variants_metadata(struct bgen_file* bgen,\
    struct bgen_vi** vi, int verbose)

    Read variants metadata and index.

    Alternatively, the user can call
    :c:func:`bgen_load_variants_metadata` to read that information from a file
    created by calling :c:func:`bgen_store_variants_metadata`.

    Remember to call :c:func:`bgen_free_variants_metadata` on the returned array
    to release allocated resources after the interaction has finished.

    :param bgen: bgen file handler.
    :param vi: variants index.
    :param verbose: ``1`` to show progress or ``0`` to disable output.
    :return: variants information.

.. c:function:: void bgen_free_variants_metadata(const struct bgen_file* bgen,\
    struct bgen_var* variants)

    Free memory associated with variants metadata.

    :param bgen: bgen file handler.
    :param variants: variants information.

.. c:function:: void bgen_free_index(struct bgen_vi* vi)

    Free memory associated with variants index.

    :param vi: variants index.

Variants metadata file
^^^^^^^^^^^^^^^^^^^^^^

.. c:function:: int bgen_store_variants_metadata(const struct bgen_file* bgen,\
    struct bgen_var *variants, struct bgen_vi *vi, const char *filepath)

    Save variants metadata into a file for faster reloading.

    :param bgen: bgen file handler.
    :param variants: variants metadata.
    :param vi: variants index.
    :param filepath: null-terminated file path to the variants metadata.

.. c:function:: struct bgen_var *bgen_load_variants_metadata(\
    const struct bgen_file* bgen, const char *filepath,\
    struct bgen_vi** vi, int verbose)

    Load variants metadata from a file.

    :param bgen: bgen file handler.
    :param filepath: null-terminated file path to the variants metadata.
    :param vi: variants index.
    :param verbose: ``1`` to show progress or ``0`` to disable output.
    :return: variants metadata.


.. c:function:: int bgen_create_variants_metadata_file(const char *bgen_fp,\
    const char *vi_fp, int verbose)

    Create variants index and save it to a file.

    :param bgen_fp: null-terminated file path to a bgen file.
    :param vi_fp: null-terminated file path to the destination file.
    :param verbose: ``1`` to show progress or ``0`` to disable output.
    :return: ``0`` on success; ``1`` otherwise.
