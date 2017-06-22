********************
Function description
********************

.. c:function:: BGenFile* bgen_open(const byte* filepath)

  Create a bgen file handler.

  :param const byte* filepath: File path to a bgen file.
  :return: Bgen file handler.
  :rtype: BGenFile*

.. c:function:: void bgen_close(BGenFile* bgen)

  Close a bgen file handler.

  :param BGenFile*: Bgen file handler.

.. c:function:: inti bgen_nsamples(BGenFile* bgen)

  Get the number of samples.

  :param BGenFile*: Bgen file handler.
  :return: Number of samples.
  :rtype: inti

.. c:function:: inti bgen_nvariants(BGenFile* bgen)

  Get the number of variants.

  :param BGenFile*: Bgen file handler.
  :return: Number of variants.
  :rtype: inti

.. c:function:: string* bgen_read_samples(BGenFile* bgen)

  Read sample identifications.

  :param BGenFile*: Bgen file handler.
  :return: Identifications.
  :rtype: string*

.. c:function:: void bgen_free_samples(BGenFile* bgen, string* samples)

  Free memory associated with sample identifications.

  :param BGenFile*: Bgen file handler.
  :param string*: Sample identifications.

.. c:function:: Variant* bgen_read_variants(BGenFile* bgen, VariantIndexing** index)

  Read variant data.
  (It does not include the probabilities.)

  :param BGenFile*: Bgen file handler.
  :param VariantIndexing**: Variant indexing.
  :return: Variant identifications, rsids, chromossomes, and more.
  :rtype: Variant*

.. c:function:: void bgen_free_variants(BGenFile* bgen, Variant* variants)

  Free memory associated with variant data.
  (It does not include the probabilities.)

  :param BGenFile*: Bgen file handler.
  :param Variant*: Variant data.

.. c:function:: void bgen_free_indexing(VariantIndexing* index)

  Free memory associated with variant indexing.

  :param BGenFile*: Bgen file handler.
  :param VariantIndexing*: Variant indexing.

.. c:function:: VariantGenotype* bgen_open_variant_genotype(VariantIndexing* index,\
                                                            inti             variant_idx)

  Return a variant reference.

  :param VariantIndexing**: Variant indexing.
  :param inti: Variant index.
  :return: Variant probabilities.
  :rtype: VariantGenotype*

.. c:function:: void bgen_read_variant_genotype(VariantIndexing* index,\
                                                VariantGenotype* vg,\
                                                real*            probabilities)

  Read the allele probabilities for a given variant.

  :param VariantIndexing*: Variant indexing.
  :param VariantGenotype*: Variant genotype handler.
  :param real*: Allele probabilities.
  :param inti: Variant index.
  :return: Variant probabilities.
  :rtype: VariantGenotype*

.. c:function:: void bgen_close_variant_genotype(VariantIndexing* index,\
                                                 VariantGenotype* vg)

  Close the variant genotype reference.

  :param VariantIndexing*: Variant indexing.
  :param VariantGenotype*: Variant genotype handler.

.. c:function:: inti bgen_sample_ids_presence(BGenFile* bgen)

  Check whether sample identifications are actually present.

  :param BGenFile*: Bgen file handler.
  :return: ``1`` for presence; ``0`` for absence.
  :rtype: inti

.. c:function:: inti bgen_variant_genotype_nsamples(VariantGenotype* vg)

  Get the number of samples.

  :param VariantGenotype*: Variant genotype handler.
  :return: Number of samples.
  :rtype: inti

.. c:function:: inti bgen_variant_genotype_nalleles(VariantGenotype* vg)

  Get the number of alleles.

  :param VariantGenotype*: Variant genotype handler.
  :return: number of alleles.
  :rtype: inti

.. c:function:: inti bgen_variant_genotype_ploidy(VariantGenotype* vg)

  Get the ploidy.

  :param VariantGenotype*: Variant genotype handler.
  :return: Ploidy.
  :rtype: inti

.. c:function:: inti bgen_variant_genotype_ncombs(VariantGenotype* vg)

  Get the number of genotype combinations.

  :param VariantGenotype*: Variant genotype handler.
  :return: Number of genotype combinations.
  :rtype: inti
