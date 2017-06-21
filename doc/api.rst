*********************
Functions description
*********************

.. c:function:: BGenFile* bgen_open(const byte* filepath)

  Create a bgen file handler.

  :param const byte* filepath: File path to the bgen file.
  :return: bgen file handler.
  :rtype: BGenFile*

.. c:function:: void bgen_close(BGenFile* bgen)

  Close a bgen file handler.

  :param BGenFile*: Bgen file handler.

.. c:function:: inti bgen_nsamples(BGenFile* bgen)

  Get the number of samples.

  :param BGenFile*: Bgen file handler.
  :return: number of samples.
  :rtype: inti

.. c:function:: inti bgen_nvariants(BGenFile* bgen)

  Get the number of variants.

  :param BGenFile*: Bgen file handler.
  :return: number of variants.
  :rtype: inti


.. c:function:: string* bgen_read_samples(BGenFile* bgen)

  Read sample identifications.

  :param BGenFile*: Bgen file handler.
  :return: identifications.
  :rtype: string*

.. c:function:: void bgen_free_samples(BGenFile* bgen, string* samples)

  Free memory associated with sample identifications.

  :param BGenFile*: Bgen file handler.
  :param string*: Sample identifications.

.. c:function:: Variant* bgen_read_variants(BGenFile* bgen, VariantIndexing** index)

  Read variant data. (It does not include the probabilities.)

  :param BGenFile*: Bgen file handler.
  :param VariantIndexing**: Variant indexing.
  :return: variant identifications, rsids, chromossomes, and more.
  :rtype: Variant*

.. c:function:: void bgen_free_variants(BGenFile* bgen, Variant* variants)

  Free memory associated with variant data.
  (It does not include the probabilities.)

  :param BGenFile*: Bgen file handler.
  :param Variant*: Variant data.

.. c:function:: void bgen_free_indexing(VariantIndexing *index)

  Free memory associated with variant indexing.

  :param BGenFile*: Bgen file handler.
  :param VariantIndexing*: Variant indexing.

.. c:function:: VariantGenotype* bgen_open_variant_genotype(VariantIndexing *indexing,
                                                inti             variant_idx)

  Get the number of variants.

  :param VariantIndexing**: Variant indexing.
  :param inti: Variant index.
  :return: variant probabilities.
  :rtype: VariantGenotype*

  .. void             bgen_read_variant_genotype(VariantIndexing *indexing,
  ..                                             VariantGenotype *vg,
  ..                                             real            *probabilities);

.. .. c:function:: inti bgen_read_genotype(BGenFile*  bgen,\
..                                                   inti   variant_idx,\
..                                                   uint32_t** ui_probs,\
..                                                   inti*  ploidy,\
..                                                   inti*  nalleles)
..
..   Read the genotype probabilities of the specified variant index.
..   Let :math:`n_a` and :math:`n_p` be the number of alleles and the ploidy.
..   The resulting matrix ``ui_probs`` will be :math:`n`-by-:math:`k`, where
..   :math:`n` is the number of samples and
..
..   .. math::
..
..     k = {n_a + n_p - 1 \choose n_a - 1}.
..
..   :param BGenFile*: Bgen file handler.
..   :param inti: Variant index.
..   :param inti**: Genotype probabilities.
..   :param inti*: Ploidy.
..   :param inti*: Number of alleles.
..
..   :return: exit status.
..   :rtype: inti




..
.. inti bgen_variant_genotype_nsamples(VariantGenotype *vg);
.. inti bgen_variant_genotype_nalleles(VariantGenotype *vg);
.. inti bgen_variant_genotype_ploidy(VariantGenotype *vg);
.. inti bgen_variant_genotype_ncombs(VariantGenotype *vg);
..
.. void bgen_close_variant_genotype(VariantIndexing *indexing,
..                                  VariantGenotype *vg);
..
.. inti bgen_sample_ids_presence(BGenFile *bgen);
