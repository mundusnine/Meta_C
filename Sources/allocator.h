#pragma once

#ifdef M9_ALLOCATOR_IMPLEMENTATION

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#define LIST_SIZE 4096 * 4096 * 4

typedef struct free_info {
    void* freed;
    const char* file;
    int line;
} free_info_t;

static uint8_t* heap = NULL;
static size_t heap_top = 0;
static free_info_t freedList[LIST_SIZE] = { 0 };
static int heap_size = 0;
void* m9_malloc(size_t size,const char *file, int line) {
    if (heap == NULL) {
        if (heap_size == 0) {
            heap_size = LIST_SIZE;
        }
        heap = (uint8_t*)malloc(heap_size);
   }
   size_t old_top = heap_top;
   heap_top += size;
   assert(heap_top <= heap_size);
   void* ptr = &heap[old_top];
   return ptr;
}

#define STB_LEAKCHECK_IMPLEMENTATION
#include "stb_leakcheck.h"

void* m9_realloc(void* ptr, size_t sz, const char* file, int line) {
    return stb_leakcheck_realloc(ptr, sz, file, line);
}

void   m9_free(void* ptr, const char* file, int line) {
    if (ptr != NULL) {
        free_info_t f = { 0 };
        int i = 0;
        for (; i < LIST_SIZE; ++i) {
            if (freedList[i].freed == 0) {
                break;
            }
            else if (freedList[i].freed == ptr) {
                f = freedList[i];
                break;
            }
        }
        if (f.freed == 0) {
            freedList[i].freed = ptr;
            freedList[i].file = file;
            freedList[i].line = line;
            stb_leakcheck_free(ptr);
        }
        else {
            fprintf(stderr, "Trying to free data in %s at line %i that was already free'd by %s at line %i", file, line, f.file, f.line);//Check double free
        }

    }
}
#else
#include "stb_leakcheck.h"
#endif // M9_ALLOCATOR_IMPLEMENTATION

// if we've already included m9_allocator before, undefine the macros
#ifdef malloc
#undef malloc
#undef free
#undef realloc
#endif

#define malloc(sz,file,line)    m9_malloc(sz,  __FILE__, __LINE__)
#define realloc(p,sz,file,line) m9_realloc(p,sz, __FILE__, __LINE__)
#define free(p) m9_free(p,__FILE__,__LINE__)

#define MetaC_Doc(...) 

extern void * m9_malloc(size_t sz, const char *file, int line);
extern void * m9_realloc(void *ptr, size_t sz, const char *file, int line);
MetaC_Doc("Free with double free checks")
extern void   m9_free(void *ptr, const char* file, int line);