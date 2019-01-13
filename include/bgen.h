/*
 * Project: bgen
 * Purpose: BGEN file reader
 * Author: Danilo Horta, danilo.horta@gmail.com
 * Language: C
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Danilo Horta
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef BGEN_H
#define BGEN_H

#define BGEN_VERSION_MAJOR 3
#define BGEN_VERSION_MINOR 0
#define BGEN_VERSION_PATCH 0
#define BGEN_VERSION "3.0.0"

#include <assert.h>
static_assert(sizeof(int) >= 4, "Code relies on int being at least 4 bytes");

#ifdef __cplusplus
extern "C"
{
#endif

#include "bgen/api.h"
#include "bgen/depr_bgen.h"
#include "bgen/depr_meta.h"
#include "bgen/file.h"
#include "bgen/geno.h"
#include "bgen/meta.h"
#include "bgen/str.h"

#ifdef __cplusplus
}
#endif

#endif /* BGEN_H */
