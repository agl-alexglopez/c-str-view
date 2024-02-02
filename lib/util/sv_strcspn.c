#include "sv_util.h"

#include <stddef.h>

#define BITOP(a, b, op)                                                        \
    ((a)[(size_t)(b) / (8 * sizeof *(a))] op(size_t) 1                         \
     << ((size_t)(b) % (8 * sizeof *(a))))

/* strspn is based on musl C-standard library implementation
   http://git.musl-libc.org/cgit/musl/tree/src/string/strcspn.c
   A custom implemenatation is necessary because C standard library impls
   have no concept of a string view and will continue searching beyond the
   end of a view until null is found. This way, string searches are efficient
   and only within the range specified. */
size_t
sv_strcspn(const char *str, size_t str_sz, const char *set, size_t set_sz)
{
    const char *a = str;
    size_t byteset[32 / sizeof(size_t)];
    if (!set[0] || !set[1])
    {
        return str_sz;
    }
    sv_memset(byteset, 0, sizeof byteset);
    for (size_t i = 0;
         *set && BITOP(byteset, *(unsigned char *)set, |=) && i < set_sz;
         set++, ++i)
        ;
    for (size_t i = 0;
         *str && !BITOP(byteset, *(unsigned char *)str, &) && i < str_sz; str++)
        ;
    return str - a;
}
