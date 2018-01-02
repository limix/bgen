function(define_sources BASE_DIR _SOURCES)

    list(APPEND REL_SOURCES util/mem.c
        util/string.c
        util/choose.c
        util/tpl.c
        util/buffer.c
        variants_index.c
        layout/two.c
        layout/one.c
        tpl/tpl.c
        zip/zstd_wrapper.c
        zip/zlib_wrapper.c
        bgen.c)

    if(CMAKE_SYSTEM_NAME MATCHES Windows)
        list(APPEND REL_SOURCES tpl/win/mmap.c)
    endif()

    limix_convert_rel_to_full(FULL_SOURCES ${BASE_DIR} "${REL_SOURCES}")
    set(${_SOURCES} ${FULL_SOURCES} PARENT_SCOPE)

endfunction(define_sources)
