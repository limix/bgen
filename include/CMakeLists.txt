function(define_public_headers BASE_DIR _PUBLIC_HEADERS _PRIVATE_HEADERS)
    set(${_PUBLIC_HEADERS} "${BASE_DIR}/bgen.h"
        PARENT_SCOPE)

    set(${_PRIVATE_HEADERS} "${BASE_DIR}/bgen/file.h;\
${BASE_DIR}/bgen/depr_bgen.h;${BASE_DIR}/bgen/depr_meta.h;\
${BASE_DIR}/bgen/str.h;${BASE_DIR}/bgen/geno.h;\
${BASE_DIR}/bgen/meta.h;${BASE_DIR}/bgen/api.h"
        PARENT_SCOPE)
endfunction(define_public_headers)
