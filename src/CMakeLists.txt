function(define_sources BASE_DIR _SOURCES)

    list(APPEND REL_SOURCES
        depr/buffer.c
        depr/tpl.c
        depr/variant.c
        depr/variants_index.c
        endian.c
        file.c
        geno.c
        io.c
        layout/one.c
        layout/two.c
        mem.c
        metafile.c
        str.c
        tpl/tpl.c
        variant.c
        zip/zlib_wrapper.c
        zip/zstd_wrapper.c)

    if(CMAKE_SYSTEM_NAME MATCHES Windows)
        list(APPEND REL_SOURCES tpl/win/mmap.c)
    endif()

    limix_convert_rel_to_full(FULL_SOURCES ${BASE_DIR} "${REL_SOURCES}")
    set(${_SOURCES} ${FULL_SOURCES} PARENT_SCOPE)

endfunction(define_sources)
