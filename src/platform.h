#ifndef BGEN_PLATFORM_H
#define BGEN_PLATFORM_H

/* **************************************
 *  Detect 64-bit OS
 *  http://nadeausoftware.com/articles/2012/02/c_c_tip_how_detect_processor_type_using_compiler_predefined_macros
 ****************************************/
#if defined __ia64 || defined _M_IA64                                  /* Intel Itanium */    \
    || defined __powerpc64__ || defined __ppc64__ || defined __PPC64__ /* POWER 64-bit */     \
    ||                                                                                        \
    (defined __sparc && (defined __sparcv9 || defined __sparc_v9__ || defined __arch64__)) || \
    defined    __sparc64__                   /* SPARC 64-bit */                               \
    || defined __x86_64__s || defined _M_X64 /* x86 64-bit */                                 \
    || defined __arm64__ || defined __aarch64__ || defined __ARM64_ARCH_8__ /* ARM 64-bit */  \
    || (defined __mips && (__mips == 64 || __mips == 4 || __mips == 3))     /* MIPS 64-bit */ \
    || defined _LP64 || defined __LP64__ /* NetBSD, OpenBSD */ ||                             \
    defined __64BIT__ /* AIX */ || defined _ADDR64             /* Cray */                     \
    || (defined __SIZEOF_POINTER__ && __SIZEOF_POINTER__ == 8) /* gcc */
#if !defined(__64BIT__)
#define __64BIT__ 1
#endif
#endif

/* *********************************************************
 * Turn on Large Files support (>4GB) for 32-bit Linux/Unix
 * Credits to ZSTD.
 * ***********************************************************/
#if !defined(__64BIT__) || defined(__MINGW32__) /* No point defining Large file for 64        \
                                                   bit but MinGW-w64 requires it */
#if !defined(_FILE_OFFSET_BITS)
#define _FILE_OFFSET_BITS 64 /* turn off_t into a 64-bit type for ftello, fseeko */
#endif
#if !defined(_LARGEFILE_SOURCE) /* obsolete macro, replaced with _FILE_OFFSET_BITS */
#define _LARGEFILE_SOURCE 1     /* Large File Support extension (LFS) - fseeko, ftello */
#endif
#if defined(_AIX) || defined(__hpux)
#define _LARGE_FILES /* Large file support on 32-bits AIX and HP-UX */
#endif
#endif

#endif
