********************
Function description
********************

.. c:function:: struct BGenFile* bgen_open(const char* filepath)

  Create a bgen file handler.

  :param const char* filepath: File path to a bgen file.
  :return: Bgen file handler.
  :rtype: struct BGenFile*

.. c:function:: void bgen_close(struct BGenFile* bgen)

  Close a bgen file handler.

  :param struct BGenFile*: Bgen file handler.

.. c:function:: int bgen_nsamples(struct BGenFile* bgen)

  Get the number of samples.

  :param struct BGenFile*: Bgen file handler.
  :return: Number of samples.
  :rtype: int

.. c:function:: int bgen_nvariants(struct BGenFile* bgen)

  Get the number of variants.

  :param struct BGenFile*: Bgen file handler.
  :return: Number of variants.
  :rtype: int

.. c:function:: string* bgen_read_samples(struct BGenFile* bgen)

  Read sample identifications.

  :param struct BGenFile*: Bgen file handler.
  :return: Identifications.
  :rtype: string*

.. c:function:: void bgen_free_samples(const struct BGenFile* bgen,
                                       string* samples)

  Free memory associated with sample identifications.

  :param const struct BGenFile*: Bgen file handler.
  :param string*: Sample identifications.

.. c:function:: struct BGVar* bgen_read_variants(struct BGenFile* bgen,
                                            struct BGenVI** index)

  Read variant data.
  (It does not include the probabilities.)

  :param struct BGenFile*: Bgen file handler.
  :param struct BGenVI**: Variant index.
  :return: Variant identifications, rsids, chromossomes, and more.
  :rtype: struct BGVar*

.. c:function:: void bgen_free_variants(struct BGenFile* bgen,
                                        struct BGVar* variants)

  Free memory associated with variant data.
  (It does not include the probabilities.)

  :param struct BGenFile*: Bgen file handler.
  :param struct BGVar*: Variant data.

.. c:function:: void bgen_free_index(struct BGenVI* index)

  Free memory associated with variant index.

  :param struct BGenVI*: Variant index.

.. c:function:: struct BGenVG* bgen_open_variant_genotype(struct BGenVI* index,
                                                          size_t variant_idx)

  Return a variant reference.

  :param struct BGenVI*: Variant index.
  :param size_t: Variant index.
  :return: Variant probabilities.
  :rtype: struct BGenVG*

.. c:function:: void bgen_read_variant_genotype(struct BGenVI* index,
                                                struct BGenVG* vg,
                                                double*  probabilities)

  Read the allele probabilities for a given variant.

  :param struct BGenVI*: Variant index.
  :param struct BGenVG*: Variant genotype handler.
  :param double*: Allele probabilities.

.. c:function:: void bgen_close_variant_genotype(struct BGenVI* index,
                                                 struct BGenVG* vg)

  Close the variant genotype reference.

  :param struct BGenVI*: Variant index.
  :param struct BGenVG*: Variant genotype handler.

.. c:function:: int bgen_sample_ids_presence(struct BGenFile* bgen)

  Check whether sample identifications are actually present.

  :param struct BGenFile*: Bgen file handler.
  :return: ``1`` for presence; ``0`` for absence.
  :rtype: int

.. c:function:: int bgen_nalleles(struct BGenVG* vg)

  Get the number of alleles.

  :param struct BGenVG*: Variant genotype handler.
  :return: number of alleles.
  :rtype: int

.. c:function:: int bgen_ploidy(struct BGenVG* vg)

  Get the ploidy.

  :param struct BGenVG*: Variant genotype handler.
  :return: Ploidy.
  :rtype: int

.. c:function:: int bgen_ncombs(struct BGenVG* vg)

  Get the number of genotype combinations.

  :param struct BGenVG*: Variant genotype handler.
  :return: Number of genotype combinations.
  :rtype: int

.. c:function:: int bgen_store_variants(const struct BGenFile *,
                                        struct BGenVar *,
                                        struct BGenVI *,
                                        const char *)

  Store variants index into a file.

  :param const struct BGenFile*: Bgen file handler.
  :param struct BGenVar*: Variant.
  :param struct BGenVI*: Variant index.
  :param const char*: File path to variants index.
  :return: ``0`` for success; ``1`` otherwise.
  :rtype: int

.. c:function:: struct BGenVar *bgen_load_variants(const struct BGenFile *,
                                                   const char *,
                                                   struct BGenVI **, int)

  Read variants index from file.
    
  :param const struct BGenFile*: Bgen file handler.
  :param const char*: File path to variants index.
  :param struct BGenVI**: Variant index.
  :param int: ``1`` for verbose output; ``0`` otherwise.
  :return: ``0`` for success; ``1`` otherwise.
  :rtype: int
