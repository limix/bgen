*********************
Functions description
*********************

.. c:function:: BGenFile* bgen_reader_open(const char* filepath)

  Create a bgen file handler.

  :param const char* filepath: File path to the bgen file.
  :return: bgen file handler.
  :rtype: BGenFile*

.. c:function:: int64_t bgen_reader_close(BGenFile* bgenfile)

  Close a bgen file handler.

  :param BGenFile*: Bgen file handler.
  :return: exit status.
  :rtype: int64_t

.. c:function:: int64_t bgen_reader_nsamples(BGenFile* bgenfile)

  Get the number of samples.

  :param BGenFile*: Bgen file handler.
  :return: number of samples.
  :rtype: int64_t

.. c:function:: int64_t bgen_reader_nvariants(BGenFile* bgenfile)

  Get the number of variants.

  :param BGenFile*: Bgen file handler.
  :return: number of variants.
  :rtype: int64_t

.. c:function:: int64_t bgen_reader_sampleid(BGenFile* bgenfile,\
                                             uint64_t  sample_idx,\
                                             unsigned char** id,\
                                             uint64_t* length)

  Get the sample id of the specified sample index.

  :param BGenFile*: Bgen file handler.
  :param uint64_t: Sample index.
  :param unsigned char**: Identification.
  :param uint64_t*: String length.

  :return: exit status.
  :rtype: int64_t

.. c:function:: int64_t bgen_reader_variantid(BGenFile* bgenfile,\
                                             uint64_t  variant_idx,\
                                             unsigned char** id,\
                                             uint64_t* length)

  Get the variant id of the specified variant index.

  :param BGenFile*: Bgen file handler.
  :param uint64_t: Variant index.
  :param unsigned char**: Identification.
  :param uint64_t*: String length.

  :return: exit status.
  :rtype: int64_t

.. c:function:: int64_t bgen_reader_variant_rsid(BGenFile* bgenfile,\
                                             uint64_t  variant_idx,\
                                             unsigned char** rsid,\
                                             uint64_t* length)

  Get the variant RSID of the specified variant index.

  :param BGenFile*: Bgen file handler.
  :param uint64_t: Variant index.
  :param unsigned char**: RSID.
  :param uint64_t*: String length.

  :return: exit status.
  :rtype: int64_t

.. c:function:: int64_t bgen_reader_variant_chrom(BGenFile* bgenfile,\
                                             uint64_t  variant_idx,\
                                             unsigned char** chrom,\
                                             uint64_t* length)

  Get the chromossome name of the specified variant index.

  :param BGenFile*: Bgen file handler.
  :param uint64_t: Variant index.
  :param unsigned char**: Chromossome name.
  :param uint64_t*: String length.

  :return: exit status.
  :rtype: int64_t

.. c:function:: int64_t bgen_reader_variant_position(BGenFile* bgenfile,\
                                             uint64_t  variant_idx,\
                                             uint64_t* position)

  Get the position of the specified variant index.

  :param BGenFile*: Bgen file handler.
  :param uint64_t: Variant index.
  :param uint64_t*: Position.

  :return: exit status.
  :rtype: int64_t

.. c:function:: int64_t bgen_reader_variant_nalleles(BGenFile* bgenfile,\
                                             uint64_t  variant_idx,\
                                             uint64_t* nalleles)

  Get the number of alleles the specified variant index has.

  :param BGenFile*: Bgen file handler.
  :param uint64_t: Variant index.
  :param nalleles*: Number of alleles.

  :return: exit status.
  :rtype: int64_t

.. c:function:: int64_t bgen_reader_variant_alleleid(BGenFile* bgenfile,\
                                             uint64_t  variant_idx,\
                                             uint64_t  allele_idx,\
                                             unsigned char** id,\
                                             uint64_t* length)

  Get the allele id of the specified allele.

  :param BGenFile*: Bgen file handler.
  :param uint64_t: Variant index.
  :param uint64_t: Allele index.
  :param unsigned char**: Identification.
  :param uint64_t*: String length.

  :return: exit status.
  :rtype: int64_t

.. c:function:: int64_t bgen_reader_read_genotype(BGenFile*  bgenfile,\
                                                  uint64_t   variant_idx,\
                                                  uint32_t** ui_probs)

  Read the genotype probabilities of the specified variant index.

  :param BGenFile*: Bgen file handler.
  :param uint64_t: Variant index.
  :param uint64_t**: Genotype probabilities.

  :return: exit status.
  :rtype: int64_t
