*********
Interface
*********

Types
^^^^^

Strings in BGEN are represented by

.. c:type:: bgen_string

    Represents a string.

    .. c:member:: int len

        String length.

    .. c:member:: char * str

        String itself. Not necessarily null-terminated.

A genetic variant is represented by the type

.. c:type:: BGenVar

    Genetic variant metadata.

    .. c:member:: bgen_string id

        Variant identifier.

    .. c:member:: bgen_string rsid

        Reference SNP cluster identifier.

    .. c:member:: int position

        Genetic position.

    .. c:member:: int nalleles

        Number of alleles.

    .. c:member:: bgen_string * allele_ids

        Array of allele identifiers.

Functions
^^^^^^^^^

.. c:function:: struct BGenFile* bgen_open(const char* filepath)

    Create a bgen file handler.

    Remember to call :c:func:`bgen_close` to close the file and release
    resources after the interaction has finished.

    :param filepath: null-terminated file path to a bgen file
    :return: bgen file handler

.. c:function:: void bgen_close(struct BGenFile* bgen)

    Close a bgen file handler.

    :param bgen: bgen file handler

.. c:function:: int bgen_nsamples(struct BGenFile* bgen)

    Get the number of samples.

    :param bgen: bgen file handler
    :return: number of samples

.. c:function:: int bgen_nvariants(struct BGenFile* bgen)

    Get the number of variants.

    :param bgen: bgen file handler
    :return: number of variants

.. c:function:: bgen_string* bgen_read_samples(struct BGenFile* bgen, int verbose)

    Read sample identifications.

    Remember to call :c:func:`bgen_free_samples` on the returned array
    to release allocated resources after the interaction has finished.

    :param bgen: bgen file handler.
    :param verbose: ``1`` to show progress or ``0`` to disable output.
    :return: identifications.

.. c:function:: void bgen_free_samples(const struct BGenFile* bgen, bgen_string* samples)

    Free memory associated with sample identifications.

    :param bgen: bgen file handler
    :param samples: identifications

.. c:function:: struct BGenVar* bgen_read_variants(struct BGenFile* bgen,\
    struct BGenVI** index, int verbose)

    Read variants metadata. Alternatively, you can call
    :c:func:`bgen_load_variants` to read that information in case variant
    headers have been stored via :c:func:`bgen_store_variants` for faster
    reloading.

    Remember to call :c:func:`bgen_free_variants` on the returned array
    to release allocated resources after the interaction has finished.

    :param bgen: bgen file handler
    :param index: variants index
    :param verbose: ``1`` to show progress or ``0`` to disable output
    :return: variants information

.. c:function:: void bgen_free_variants(const struct BGenFile* bgen,\
    struct BGenVar* variants)

    Free memory associated with variants metadata.

    :param bgen: bgen file handler
    :param variants: variants information

.. c:function:: void bgen_free_index(struct BGenVI* index)

    Free memory associated with variants index.

    :param index: variants index

.. c:function:: struct BGenVG* bgen_open_variant_genotype(struct BGenVI* vi,\
    size_t index)

    Return a variant reference. Remember to call
    :c:func:`bgen_close_variant_genotype` to the returned reference after
    the interaction has finished.

    :param vi: variants index
    :param index: array index of the requested variant
    :return: variant probabilities

.. c:function:: void bgen_read_variant_genotype(struct BGenVI* index,\
    struct BGenVG* vg,\
    double* probs)

    Read the allele probabilities for a given variant.

    :param index: variants index
    :param vg: variant genotype handler
    :param probs: allele probabilities

.. c:function:: void bgen_close_variant_genotype(struct BGenVI* index,\
    struct BGenVG* vg)

    Close the variant genotype reference.

    :param index: variants index
    :param vg: variant genotype handler

.. c:function:: int bgen_sample_ids_presence(struct BGenFile* bgen)

    Check whether sample identifications are actually present.

    :param bgen: bgen file handler
    :return: ``1`` for presence; ``0`` for absence.

.. c:function:: int bgen_nalleles(struct BGenVG* vg)

    Get the number of alleles.

    :param vg: variant genotype handler
    :return: number of alleles

.. c:function:: int bgen_ploidy(struct BGenVG* vg)

    Get the ploidy.

    :param vg: variant genotype handler
    :return: ploidy

.. c:function:: int bgen_ncombs(struct BGenVG *vg)

    Get the number of genotype combinations for a given variant.

    :param vg: variant genotype handler.
    :return: number of possible genotypes

Variants metadata file
^^^^^^^^^^^^^^^^^^^^^^

.. c:function:: int bgen_store_variants(const struct BGenFile* bgen,\
    struct BGenVar *variants, struct BGenVI *index, const char *filepath)

    Save variants metadata into a file for faster reloading.

    :param bgen: bgen file handler
    :param variants: variants metadata.
    :param index: variants index.
    :param filepath: null-terminated file path to the variants metadata cache.

.. c:function:: struct BGenVar *bgen_load_variants(\
    const struct BGenFile* bgen, const char *filepath,\
    struct BGenVI** index, int verbose)

    Load variants metadata from a file.

    :param bgen: bgen file handler.
    :param filepath: null-terminated file path to the variants metadata cache.
    :param index: variants index.
    :param verbose: ``1`` to show progress or ``0`` to disable output.
    :return: variants metadata.


.. c:function:: int bgen_create_variants_index_file(const char *bgen_fp,\
    const char *index_fp, int verbose)

    Create variants index and save it to a file.

    :param bgen: null-terminated file path to a bgen file.
    :param filepath: null-terminated file path to the destination file.
    :param verbose: ``1`` to show progress or ``0`` to disable output.
    :return: ``0`` on success; ``1`` otherwise.
