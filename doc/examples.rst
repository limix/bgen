********
Examples
********

.. literalinclude :: ../example/create_metafile.c
   :language: c

Output::

   Finished successfully.

.. literalinclude :: ../example/read_samples.c
   :language: c

Output::

   Number of samples: 500
   Number of variants: 199
   First sample ID: sample_001
   Second sample ID: sample_002


.. literalinclude :: ../example/read_metadata.c
   :language: c

Output::

   Number of variants in partition 0: 100
   Number of variants in partition 1: 99
   First variant id: SNPID_2
   First variant rsid: SNPID_
   First variant chrom: 01
   Number of alleles: 2
   First allele: A
   Second allele: A

.. literalinclude :: ../example/read_probability.c
   :language: c

Output::

   We've got vaddr 345403 for variant SNPID_105.
   Ploidy for sample 23: 2.
   Genotype is unphased.
   Genotype probabilities for sample 23:
   0.966736 0.028381 0.004883
