/*
    Made by Noah Van Miert
    6/12/2022

    Apollo Compiler
*/

#ifndef __CORE_H_
#define __CORE_H_

#include <assert.h>
#include <stddef.h>

#define todo()                  assert(0 && "not implemented")

#define bool _Bool
#define true 1
#define false 0

/* likely - This will indicate that a value is mostly going to be 1 */
#ifndef likely
    #define likely(x)           __builtin_expect(!!(x), 1)
#endif

/* unlikely - This will indicate that a value is mostly going to be 0 */
#ifndef unlikely
    #define unlikely(x)         __builtin_expect(!!(x), 0)
#endif

/*
    Some wrappers around the allocate functions
    so it will exit when the allocation
    failed.
*/
void *xmalloc(size_t size);
void *xcalloc(size_t nmemb, size_t size);
void *xrealloc(void *_ptr, size_t size);

#endif 