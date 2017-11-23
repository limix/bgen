*********
Functions
*********

.. c:function:: struct BGenFile* bgen_open(const byte* filepath)

    Create a bgen file handler.

    :param filepath: File path to a bgen file
    :return: Bgen file handler

.. c:function:: void bgen_close(struct BGenFile* bgen)

    Close a bgen file handler.

    :param bgen: Bgen file handler

.. c:function:: int bgen_nsamples(struct BGenFile* bgen)

    Get the number of samples.

    :param bgen: Bgen file handler
    :return: number of samples

.. c:function:: int bgen_nvariants(struct BGenFile* bgen)

    Get the number of variants.

    :param bgen: Bgen file handler
    :return: number of variants

.. c:function:: string* bgen_read_samples(struct BGenFile* bgen)

    Read sample identifications.

    :param bgen: Bgen file handler
    :return: identifications

.. c:function:: void bgen_free_samples(struct BGenFile* bgen, string* samples)

    Free memory associated with sample identifications.

    :param bgen: Bgen file handler
    :param samples: Identifications

.. c:function:: struct BGenVar* bgen_read_variants(struct BGenFile* bgen,\
    struct BGenVI** index)

    Read variant data.

    :param bgen: Bgen file handler
    :param index: Variants index
    :return: variants information

.. c:function:: void bgen_free_variants(struct BGenFile* bgen,\
    BGenVar* variants)

    Free memory associated with variant data.

    :param bgen: Bgen file handler
    :param variants: Variants information

.. c:function:: void bgen_free_indexing(struct BGenVI* index)

    Free memory associated with variants index.

    :param index: Variants index

.. c:function:: struct BGenVG* bgen_open_variant_genotype(struct BGenVI* vi,\
    int index)

    Return a variant reference.

    :param vi: Variants index
    :param index: Variant index
    :return: variant probabilities

.. c:function:: void bgen_read_variant_genotype(struct BGenVI* index,\
    struct BGenVG* vg,\
    double* probs)

    Read the allele probabilities for a given variant.

    :param index: Variants index
    :param vg: Variant genotype handler
    :param probs: Allele probabilities

.. c:function:: void bgen_close_variant_genotype(struct BGenVI* index,\
    struct BGenVG* vg)

    Close the variant genotype reference.

    :param index: Variants index
    :param vg: Variant genotype handler

.. c:function:: int bgen_sample_ids_presence(struct BGenFile* bgen)

    Check whether sample identifications are actually present.

    :param bgen: Bgen file handler
    :return: ``1`` for presence; ``0`` for absence.

.. c:function:: int bgen_nalleles(struct BGenVG* vg)

    Get the number of alleles.

    :param vg: Variant genotype handler
    :return: number of alleles

.. c:function:: int bgen_ploidy(struct BGenVG* vg)

    Get the ploidy.

    :param vg: Variant genotype handler
    :return: ploidy

