function(define_public_headers BASE_DIR _PUBLIC_HEADERS)
    set(${_PUBLIC_HEADERS} "${BASE_DIR}/bgen/bgen.h;${BASE_DIR}/bgen/string.h"
        PARENT_SCOPE)
endfunction(define_public_headers)
