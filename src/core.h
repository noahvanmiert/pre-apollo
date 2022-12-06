#ifndef __CORE_H_
#define __CORE_H_

#include <assert.h>

#define nullptr (void *)0

#define assert_memory(ptr) assert(ptr && "apollo: could not allocate enough memory")

#endif