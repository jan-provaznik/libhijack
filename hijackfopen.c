/* 2018 - 2022 Jan Provaznik (jan@provaznik.pro)
 *
 * Intercepts fopen(path, mode) calls. 
 *
 * If path matches HIJACK_FOPEN_SOURCE, 
 * replace it with HIJACK_FOPEN_TARGET.
 *
 * Supports only a single path.
 */

#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEF_HIJACK_FOPEN_SOURCE "HIJACK_FOPEN_SOURCE"
#define DEF_HIJACK_FOPEN_TARGET "HIJACK_FOPEN_TARGET"

static FILE * 
(* fopen_next) (
  const char * path,
  const char * mode
) = 0;

FILE *
fopen (
  const char * path,
  const char * mode
) {

  if (! fopen_next) {
    fopen_next = dlsym(RTLD_NEXT, "fopen");
  }

  if (! fopen_next) {
    return 0;
  }

  char * source = getenv(DEF_HIJACK_FOPEN_SOURCE);
  char * target = getenv(DEF_HIJACK_FOPEN_TARGET);

  if (source && target && 0 == strcmp(source, path)) {
    return fopen_next(target, mode);
  }

  return fopen_next(path, mode);
}

// __attribute__((constructor))
// static void libinit () {
// }

/* cc -fPIC -shared -ldl hijackfopen -o libhijackfopen.so */

