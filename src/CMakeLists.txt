function(define_sources BASE_DIR _SOURCES)

    list(APPEND REL_SOURCES
        bgen.c
        index.c
        layout/one.c
        layout/two.c
        tpl/tpl.c
        util/buffer.c
        util/choose.c
        util/endian.c
        util/int.c
        util/mem.c
        util/str.c
        util/tpl.c
        variants_index.c
        zip/zlib_wrapper.c
        zip/zstd_wrapper.c)

    if(CMAKE_SYSTEM_NAME MATCHES Windows)
        list(APPEND REL_SOURCES tpl/win/mmap.c)
    endif()

    limix_convert_rel_to_full(FULL_SOURCES ${BASE_DIR} "${REL_SOURCES}")
    set(${_SOURCES} ${FULL_SOURCES} PARENT_SCOPE)

endfunction(define_sources)
