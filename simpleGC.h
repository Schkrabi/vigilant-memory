/*
 * Simple Garbage Collector as it is described in the Matthew Plant's blog
 * (see http://web.engr.illinois.edu/~maplant2/gc.html)
 * 
 * @author Mgr. Radomir Skrabal (well I am really just rewriting what I 
 * 		see and trying to make sense of it) (correction, ok I am not
 * 		doing the stupind GC on the blog, but I use some basic ideas
 * 		and maybe a little code)
 */

#include <limits.h>

/**
 * Type for pointer bit-wise op. (&, | etc.)
 */
typedef unsigned long long ptr_int;

/**
 * Tags the block ptr into the least significant bit (aligned, normally 0x0) of the next pointer
 */
#define TAG(block_ptr)		tag(block_ptr);
#define UNTAG_MASK		ULONG_MAX - 1

/**
 * Untagged value of ptr
 */
#define UNTAG(ptr) 		(header_t*)(((ptr_int) (ptr)) & UNTAG_MASK) 

/**
 * Allocation unit, one page
 */
#define MIN_ALLOC_SIZE 4096

/**
 * Allocation alling unit
 * Platform dependent on linux sizeof(void*) is a size of a WORD
 */
#define ALLOC_ALIGN sizeof(void*)

/**
 * Header describing the block of memory.
 * Should be at the beggining of each block.
 */
typedef struct header {
	size_t		size;  //Size in bytes, including bytes used by header
	struct header	*next; //next header in list 
} header_t;

void tag(header_t *block_ptr);
int is_tagged(header_t *block_ptr);
size_t available_size(header_t *block);
void *start_of_block(header_t *block_ptr);
void *end_of_block(header_t *block_ptr);
header_t *next_block(header_t*);

/**
 * Start of the free memory block list
 */
extern header_t *freeptr;
/**
 * Start of the used memory block list
 */
extern header_t *usedptr;

/**
 * Bottom of a stack
 */
extern void *stack_bottom;
/**
 * Top of a stack
 */
extern void *stack_top;
/**
 * Start of the uninitialized data segment
 */
extern void *BBSstart;
/**
 * End of the uninitilized data segment
 */
extern void *BBSend;

size_t aling_by_size(size_t n, size_t aling_by);
void* aling_pointer(void* n, size_t aling_by);
int is_allinged(void *);

void add_to_list(header_t **list_ptr, header_t *new_ptr);
header_t *remove_from_list(header_t **list_ptr, header_t *previous_ptr, header_t *removed_ptr);
int is_empty(header_t **list_ptr);

header_t *first_fit(header_t **list_ptr, size_t size);

header_t *morecore(size_t size);
void * GC_malloc(size_t alloc_size);

void mark_from_region(void *start_ptr, void *end_ptr);
void mark_from_heap(void);

size_t read_line(FILE *file, char* buffer, size_t max_size);
int get_stack_line(char *buffer, size_t max_size);
void *get_stack_bottom();
/**
 * Refreshes the stack_top variable
 */
#define REFRESH_STACK_TOP asm volatile ("mov %%rbp, %0" : "=r" (stack_top));

int GC_init(void);
//Up till here tested
void GC_collect(void);