function(define_public_headers BASE_DIR _PUBLIC_HEADERS)
    set(${_PUBLIC_HEADERS} "${BASE_DIR}/bgen.h;${BASE_DIR}/file.h;${BASE_DIR}/depr.h;
                            ${BASE_DIR}/str.h;${BASE_DIR}/geno.h;${BASE_DIR}/meta.h;
                            ${BASE_DIR}/api.h"
        PARENT_SCOPE)
endfunction(define_public_headers)
