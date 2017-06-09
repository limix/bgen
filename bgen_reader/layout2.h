#ifndef LAYOUT2_H
#define LAYOUT2_H

int64_t bgen_genotype_block_layout2(BGenFile     *bgenfile,
                                    int64_t       compression,
                                    int64_t       nsamples,
                                    VariantBlock *vb,
                                    uint32_t     *ui_probs);

#endif /* ifndef LAYOUT2_H */
