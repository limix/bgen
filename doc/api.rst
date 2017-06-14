*********************
Functions description
*********************

.. c:function:: BGenFile* bgen_reader_open(const char* filepath)

  Create a bgen file handler.

  :param const char* filepath: File path to the bgen file.
  :return: bgen file handler.
  :rtype: BGenFile*

.. c:function:: inti bgen_reader_close(BGenFile* bgenfile)

  Close a bgen file handler.

  :param BGenFile*: Bgen file handler.
  :return: exit status.
  :rtype: inti

.. c:function:: inti bgen_reader_nsamples(BGenFile* bgenfile)

  Get the number of samples.

  :param BGenFile*: Bgen file handler.
  :return: number of samples.
  :rtype: inti

.. c:function:: inti bgen_reader_nvariants(BGenFile* bgenfile)

  Get the number of variants.

  :param BGenFile*: Bgen file handler.
  :return: number of variants.
  :rtype: inti

.. c:function:: inti bgen_reader_sampleid(BGenFile* bgenfile,\
                                             inti  sample_idx,\
                                             unsigned char** id,\
                                             inti* length)

  Get the sample id of the specified sample index.

  :param BGenFile*: Bgen file handler.
  :param inti: Sample index.
  :param unsigned char**: Identification.
  :param inti*: String length.

  :return: exit status.
  :rtype: inti

.. c:function:: inti bgen_reader_variantid(BGenFile* bgenfile,\
                                             inti  variant_idx,\
                                             unsigned char** id,\
                                             inti* length)

  Get the variant id of the specified variant index.

  :param BGenFile*: Bgen file handler.
  :param inti: Variant index.
  :param unsigned char**: Identification.
  :param inti*: String length.

  :return: exit status.
  :rtype: inti

.. c:function:: inti bgen_reader_variant_rsid(BGenFile* bgenfile,\
                                             inti  variant_idx,\
                                             unsigned char** rsid,\
                                             inti* length)

  Get the variant RSID of the specified variant index.

  :param BGenFile*: Bgen file handler.
  :param inti: Variant index.
  :param unsigned char**: RSID.
  :param inti*: String length.

  :return: exit status.
  :rtype: inti

.. c:function:: inti bgen_reader_variant_chrom(BGenFile* bgenfile,\
                                             inti  variant_idx,\
                                             unsigned char** chrom,\
                                             inti* length)

  Get the chromossome name of the specified variant index.

  :param BGenFile*: Bgen file handler.
  :param inti: Variant index.
  :param unsigned char**: Chromossome name.
  :param inti*: String length.

  :return: exit status.
  :rtype: inti

.. c:function:: inti bgen_reader_variant_position(BGenFile* bgenfile,\
                                             inti  variant_idx,\
                                             inti* position)

  Get the position of the specified variant index.

  :param BGenFile*: Bgen file handler.
  :param inti: Variant index.
  :param inti*: Position.

  :return: exit status.
  :rtype: inti

.. c:function:: inti bgen_reader_variant_nalleles(BGenFile* bgenfile,\
                                             inti  variant_idx,\
                                             inti* nalleles)

  Get the number of alleles the specified variant index has.

  :param BGenFile*: Bgen file handler.
  :param inti: Variant index.
  :param nalleles*: Number of alleles.

  :return: exit status.
  :rtype: inti

.. c:function:: inti bgen_reader_variant_alleleid(BGenFile* bgenfile,\
                                             inti  variant_idx,\
                                             inti  allele_idx,\
                                             unsigned char** id,\
                                             inti* length)

  Get the allele id of the specified allele.

  :param BGenFile*: Bgen file handler.
  :param inti: Variant index.
  :param inti: Allele index.
  :param unsigned char**: Identification.
  :param inti*: String length.

  :return: exit status.
  :rtype: inti

.. c:function:: inti bgen_reader_read_genotype(BGenFile*  bgenfile,\
                                                  inti   variant_idx,\
                                                  uint32_t** ui_probs,\
                                                  inti*  ploidy,\
                                                  inti*  nalleles)

  Read the genotype probabilities of the specified variant index.
  Let :math:`n_a` and :math:`n_p` be the number of alleles and the ploidy.
  The resulting matrix ``ui_probs`` will be :math:`n`-by-:math:`k`, where
  :math:`n` is the number of samples and

  .. math::

    k = {n_a + n_p - 1 \choose n_a - 1}.

  :param BGenFile*: Bgen file handler.
  :param inti: Variant index.
  :param inti**: Genotype probabilities.
  :param inti*: Ploidy.
  :param inti*: Number of alleles.

  :return: exit status.
  :rtype: inti
