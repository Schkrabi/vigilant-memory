/*
 * Simple Garbage Collector as it is described in the Matthew Plant's blog
 * (see http://web.engr.illinois.edu/~maplant2/gc.html)
 * 
 * @author Mgr. Radomir Skrabal (well I am really just rewriting what I 
 * 		see and trying to make sense of it) (correction, ok I am not
 * 		doing the stupind GC on the blog, but I use some basic ideas
 * 		and maybe a little code)
 */

#include <unistd.h> //sbrk
#include <stdio.h> // For testing
#include <stdlib.h> //For statck bottom search, to be replaced
#include <assert.h>
#include "simpleGC.h"

/**
 * Tags the block
 * @arg tagged block
 */
void tag(header_t *block_ptr)
{
	block_ptr->next = (header_t*)((ptr_int)block_ptr->next | 0x1);
}

/**
 * Yields 1 if block_ptr is tagged, otherwise yields 0 (false)
 * @param block_ptr inspecte block
 * @return 1 (true) or 0 (false)
 */
int is_tagged(header_t *block_ptr)
{
	return block_ptr != NULL && (ptr_int)block_ptr->next & 0x1;
}

/**
 * Vrací velikost využitelného místa v bloku
 * @arg blok paměti
 * @return počet využitelných bytů
 */
size_t available_size(header_t *block)
{
	return block->size - sizeof(header_t);
}

/**
 * For block determines the word-aligned start of the usable memory
 * @arg block_ptr block of memory
 * @return pointer to start of a usable memory of block_ptr
 */
void *start_of_block(header_t *block_ptr)
{
	return block_ptr + 1;
}

/**
 * Determines the word-aligned end of a block
 * @arg block_ptr block of memory
 * @return word-aligned pointer to a end of memory block
 */
void *end_of_block(header_t *block_ptr)
{
	return (void*)((ptr_int)block_ptr + block_ptr->size);
}

/**
 * Returns next block of memory, or NULL if there is none
 * @arg block_ptr current memory block
 * @return next memory block in list or NULL
 */
header_t *next_block(header_t* block_ptr)
{
	return (header_t*)((ptr_int)block_ptr->next & UNTAG_MASK);
}

/**
 * Alings first argument to the closest greater multiple of the second argument
 * @arg n alingned number
 * @arg aling_by numer by which is aligned
 */
size_t aling_by_size(size_t n, size_t aling_by)
{
	return ((n / aling_by) + ((n % aling_by) > 0)) * aling_by;
}

/**
 * Alings first argument to the closest greater multiple of the second argument
 * @arg n alingned number
 * @arg aling_by numer by which is aligned
 */
void* aling_pointer(void* ptr, size_t aling_by)
{
	void *tmp = (void*)((ptr_int)ptr & (ULONG_MAX & ~(ptr_int)aling_by));
	if(ptr < tmp)
		return (void*)((ptr_int)tmp + aling_by);
	return tmp;
}

/**
 * Vrací true jestliže je ptr zarovnaný na násobek ALLOC_ALIGN jinak vrací false
 * @arg ptr pointer
 * @see ALLOC_ALIGN
 * @return 1 (true) or 0 (false)
 */
int is_allinged(void *ptr)
{
	return (ptr_int)ptr % ALLOC_ALIGN == 0;
}

/**
 * Appends new block to end of the list
 * @arg list_ptr pointer to the start pointer of a list
 * @arg new_ptr pointer to the new block of memory added
 */
void add_to_list(header_t **list_ptr, header_t *new_ptr)
{
	header_t *current_ptr, *previous_ptr = NULL;
	
	for(current_ptr = *list_ptr; current_ptr != NULL; current_ptr = next_block(current_ptr))
	{
		previous_ptr = current_ptr;
	}
	
	if(previous_ptr == NULL)
		*list_ptr = new_ptr;
	else
		previous_ptr->next = new_ptr;
	
	new_ptr->next = NULL;
}

/**
 * Removes block from the list
 * @arg list_ptr pointer to the start pointer of a list
 * @arg previous_ptr pointer to previous block in list before removed_ptr
 * @arg removed_ptr pointer to removed block in list
 * @return A first block that yields pred(block) == 1 or NULL if none such block exists
 */
header_t *remove_from_list(header_t **list_ptr, header_t *previous_ptr, header_t *removed_ptr)
{
	if(removed_ptr != NULL)
	{
		//A block was on the start of the list
		if(previous_ptr == NULL)
		{
			*list_ptr = removed_ptr->next;
		}
		else
		{
			previous_ptr->next = removed_ptr->next;
		}
		removed_ptr->next = NULL;
	}
	
	return removed_ptr;
}

/**
 * True if list is empty, false otherwise
 * @arg list_ptr pointer to the start pointer of a list
 * @return 1 (true) or 0 (false)
 */
int is_empty(header_t **list_ptr)
{
	return (*list_ptr) == NULL;
}

/**
 * Retrieves from the list first block of fitting size
 * @arg list_ptr pointer to the start pointer of a list
 * @arg size minimum searched size
 */
header_t *first_fit(header_t **list_ptr, size_t size)
{
	header_t *previous_ptr = NULL, *current_ptr;
	
	for(current_ptr = *list_ptr; current_ptr != NULL; current_ptr = next_block(current_ptr))
	{
		if(available_size(current_ptr) >= size)
			break;
		previous_ptr = current_ptr;
	}
	
	return remove_from_list(list_ptr, previous_ptr, current_ptr);
}

/**
 * Request more memory from the kernel
 * Returns aligned memory
 * @param size number of bytes requested from the kernel
 * @see header
 */
header_t *morecore(size_t size)
{
	void 		*sys_ret;
	header_t 	*block_ptr;
	size_t		alloc_size;
	size_t		unit_size;
	
	//Allocating minimum one page
	if(size + sizeof(header_t) < MIN_ALLOC_SIZE)
	{
		alloc_size = MIN_ALLOC_SIZE;
	}
	else
	{
		alloc_size = aling_by_size(size + sizeof(header_t), ALLOC_ALIGN);
	}
	
	//Call the system and initialize memory block
	block_ptr = sbrk(0);
	
	//Unlikely, if the begining of the memory wasn't aligned
	if(!is_allinged((void*)block_ptr))
	{
		void *tmp, *r;
		tmp = aling_pointer((void*)block_ptr, ALLOC_ALIGN);
		r = sbrk((ptr_int)tmp - (ptr_int)block_ptr);
		if(r == (void*)-1)
		{
			return NULL;
		}
		block_ptr = sbrk(0);
	}
	
	sys_ret = sbrk(alloc_size);
	if(sys_ret == (void*) -1)
	{
		return NULL;
	}
	
	block_ptr->size = alloc_size;
	block_ptr->next = NULL;
	
	return block_ptr;
}

/**
 * Malloc for garbage collection, uses first fit
 * @arg alloc_size size of allocated memory (in bytes)
 */
void * GC_malloc(size_t alloc_size)
{
	header_t *block_ptr;
	
	block_ptr = first_fit(&freeptr, alloc_size);
	
	if(block_ptr == NULL)
	{
		block_ptr = morecore(alloc_size);
	}
	
	//Add to used blocks
	add_to_list(&usedptr, block_ptr);
	
	//Give word aligned memory
	return start_of_block(block_ptr);
}

/**
 * Scan a region of memory and mark any items in the used list appropriately.
 * Both arguments should be word aligned.
 */
static void mark_from_region(void *start_ptr, void *end_ptr)
{
	header_t 	*block_ptr;
	void	 	*current_ptr;
	
	//Iterate word-wise throught the memory
	for(current_ptr = start_ptr; current_ptr < end_ptr; current_ptr += sizeof(void*))
	{
		unsigned int value;
		
		//printf("Inspecting ptr %x\n", current_ptr);
		value = *(unsigned int*)current_ptr;
		
		//printf("Seachring for memory with adress %x\n", current_ptr);
		
		//Iterate thought used memory blocks
		block_ptr = usedptr;
		do
		{
			//If pointer value point somewhere into this allocate block
			if((unsigned int)start_of_block(block_ptr) <= value
				&& (unsigned int)start_of_block(block_ptr) + block_ptr->size > value)
			{
				printf("Tagging block %x\n", block_ptr);
				TAG(block_ptr)
				break;
			}
			block_ptr = UNTAG(block_ptr->next);
		}while(block_ptr != NULL);
	}
}

/**
 * Scans the heap for active pointers and marks ones that points somewhere
 */
static void mark_from_heap(void)
{
	header_t *current_ptr;
	
	for(current_ptr = usedptr; current_ptr != NULL; current_ptr = UNTAG(current_ptr->next))
	{
		mark_from_region(start_of_block(current_ptr), end_of_block(current_ptr));
	}
}

/*
 * Find the absolute bottom of the stack and set stuff up.
 * Pretty stupid way, at least find a way to use the system call for this (later)
 */
void GC_init(void)
{
    static int initted;
    FILE *statfp;

    if (initted)
        return;

    initted = 1;

    statfp = fopen("/proc/self/stat", "r");
    assert(statfp != NULL);
    fscanf(statfp,
           "%*d %*s %*c %*d %*d %*d %*d %*d %*u "
           "%*lu %*lu %*lu %*lu %*lu %*lu %*ld %*ld "
           "%*ld %*ld %*ld %*ld %*llu %*lu %*ld "
           "%*lu %*lu %*lu %lu", &stack_bottom);
    fclose(statfp);

    usedptr = NULL;
}

/**
 * Main garbage collector function
 */
void GC_collect(void)
{
	header_t *current_ptr, *previous_ptr = NULL;
	int stack_top;
	void *start;
	extern char end, etext; // Provided by the linker.
	
	if(is_empty(&usedptr))
		return;
	
	//TODO make this work
	//start = &etext + (sizeof(void*) - ((int)&etext % sizeof(void*)));
	
	// Scan the BSS and initialized data segments. 
	//mark_from_region(start, &end);

	// Scan the stack.
	asm volatile ("movl %%ebp, %0" : "=r" (stack_top));
	
	printf("\nMARKING FROM STACK\n\n");
	mark_from_region(stack_bottom, (void*)stack_top);

	// Mark from the heap.
	printf("\nMARKING FROM HEAP\n\n");
	mark_from_heap();
	
	// Collect 
	current_ptr = usedptr;
	while(current_ptr != NULL)
	{
		if(!is_tagged(current_ptr))
		{
			header_t *block_ptr;
			
			block_ptr = remove_from_list(&usedptr, previous_ptr, current_ptr);
			
			printf("Found unmarked block %x\n", block_ptr);
			
			add_to_list(&freeptr, block_ptr);
			
			if(previous_ptr != NULL)
				current_ptr = previous_ptr;
			else
			{
				current_ptr = usedptr;
				continue;
			}
			
		}
		previous_ptr = current_ptr;
		current_ptr = current_ptr->next;
	}
}