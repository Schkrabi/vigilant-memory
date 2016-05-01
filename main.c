#include <stdlib.h>
#include <stdio.h>
#include "simpleGC.h"

/**
 * Testing entry point
 */
int main(int argc, char *argv[])
{	
	/*int *ptr1, *ptr2, *ptr3;
	header_t *block_ptr;
	
	GC_init();
	
	ptr1 = GC_malloc(sizeof(int));
	//ptr2 = GC_malloc(300 * sizeof(int));
	//ptr3 = GC_malloc(1000 * sizeof(int));
	
	printf("ptr1 address %x\n", ptr1);
	
	printf("Listing out the used list:\n");
	for(block_ptr = usedptr; block_ptr != NULL; block_ptr = block_ptr->next)
	{
		printf("block_ptr: %x; ", block_ptr);
		printf("start: %x; ", start_of_block(block_ptr));
		printf("end: %x\n", end_of_block(block_ptr));
	}
	printf("Listing out the free list:\n");
	for(block_ptr = freeptr; block_ptr != NULL; block_ptr = block_ptr->next)
	{
		printf("block_ptr: %x; ", block_ptr);
		printf("start: %x; ", start_of_block(block_ptr));
		printf("end: %x\n", end_of_block(block_ptr));
	}
	
	printf("Calling GC_collect\n");
	GC_collect();
	
	printf("Listing out the used list:\n");
	for(block_ptr = usedptr; block_ptr != NULL; block_ptr = block_ptr->next)
	{
		printf("block_ptr: %x; ", block_ptr);
		printf("start: %x; ", start_of_block(block_ptr));
		printf("end: %x\n", end_of_block(block_ptr));
	}
	printf("Listing out the free list:\n");
	for(block_ptr = freeptr; block_ptr != NULL; block_ptr = block_ptr->next)
	{
		printf("block_ptr: %x; ", block_ptr);
		printf("start: %x; ", start_of_block(block_ptr));
		printf("end: %x\n", end_of_block(block_ptr));
	}
	
	printf("Nullifiing the ptr2.\n");
	ptr2 = NULL;
	
	printf("Calling GC_collect\n");
	GC_collect();	
	
	printf("Listing out the used list:\n");
	for(block_ptr = usedptr; block_ptr != NULL; block_ptr = block_ptr->next)
	{
		printf("block_ptr: %x; ", block_ptr);
		printf("start: %x; ", start_of_block(block_ptr));
		printf("end: %x\n", end_of_block(block_ptr));
	}
	printf("Listing out the free list:\n");
	for(block_ptr = freeptr; block_ptr != NULL; block_ptr = block_ptr->next)
	{
		printf("block_ptr: %x; ", block_ptr);
		printf("start: %x; ", start_of_block(block_ptr));
		printf("end: %x\n", end_of_block(block_ptr));
	}
	
	return 0;*/
	
	header_t h, i, j, k;
	header_t *list, *it, *ptr, *b1, *b2, *b3, *b4;
	void* l;
	size_t s;
	
	char *charp;
	int *intp, i1;
	short *shortp;
	
	/*//Basic info
	printf("%d\n", sizeof(h));
	printf("%d\n", sizeof(header_t));
	printf("%d\n", sizeof(size_t));
	printf("%d\n", sizeof(header_t*));
	
	printf("\n");
	
	//Testing list
	list = NULL;
	h.size = 101;
	i.size = 102;
	j.size = 103;
	k.size = 104;
	
	add_to_list(&list, &h);
	add_to_list(&list, &i);
	add_to_list(&list, &j);
	add_to_list(&list, &k);
	
	for(it = list; it != NULL; it = next_block(it))
		printf("%d\n", it->size);
	printf("\n");
	
	remove_from_list(&list, &i, &j);
	
	for(it = list; it != NULL; it = next_block(it))
		printf("%d\n", it->size);
	printf("\n");
	
	//Testing First fit
	ptr = first_fit(&list, 200);
	
	printf("%x\n", ptr);
	
	ptr = first_fit(&list, 102 - sizeof(header_t));
	
	printf("%x\n", ptr);
	printf("%d\n", ptr->size);
	
	for(it = list; it != NULL; it = next_block(it))
		printf("%d\n", it->size);
	
	printf("\n");
	
	//Testing aling_by_size and aling_pointer
	printf("%d\n", aling_by_size(3, 4));
	printf("%d\n", aling_pointer((void*)3, 4));
	s = aling_by_size((ptr_int)&h, ALLOC_ALIGN);
	l = aling_pointer(&h, ALLOC_ALIGN);
	printf("%x\n", s);
	printf("%x\n", l);
	printf("%x\n", &h);
	
	
	//Testing morecore
	b1 = morecore(1);
	b2 = morecore(MIN_ALLOC_SIZE);
	b3 = morecore(MIN_ALLOC_SIZE + 5);
	b4 = morecore(4 * MIN_ALLOC_SIZE);
	
	printf("%x: size %d, next: %x\n", b1, b1->size, b1->next);
	printf("%x: size %d, next: %x\n", b2, b2->size, b2->next);
	printf("%x: size %d, next: %x\n", b3, b3->size, b3->next);
	printf("%x: size %d, next: %x\n", b4, b4->size, b4->next);
	printf("\n");
	
	//Tag test
	tag(b1);
	tag(b3);
	
	printf("%x: size %d, next: %x\n", b1, b1->size, b1->next);
	printf("%x: size %d, next: %x\n", b2, b2->size, b2->next);
	printf("%x: size %d, next: %x\n", b3, b3->size, b3->next);
	printf("%x: size %d, next: %x\n", b4, b4->size, b4->next);
	printf("\n");
	
	printf("%d\n", is_tagged(b1));
	printf("%d\n", is_tagged(b2));
	printf("%d\n", is_tagged(b3));
	printf("%d\n", is_tagged(b4));
	printf("\n");
	
	printf("%d\n", next_block(b1));
	printf("%d\n", next_block(b2));
	printf("%d\n", next_block(b3));
	printf("%d\n", next_block(b4));
	*/
	
	//GC_malloc test
	charp = (char*)GC_malloc(6 * sizeof(char));
	charp[0] = 'H';
	charp[1] = 'e';
	charp[2] = 'l';
	charp[3] = 'l';
	charp[4] = 'o';
	charp[5] = '\0';
	
	printf("%s\n", charp);
	
	intp = (int*)GC_malloc(1000 * sizeof(int));
	for(i1 = 0; i1 < 1000; i1++)
		printf("%d", intp[i1]);
	printf("\n");
	
	shortp = (short*)GC_malloc(sizeof(short));
	*shortp = 4;
	printf("%x: %d\n", shortp, *shortp);
	
	//No data, static reference problem?
	for(it = usedptr; it != NULL; it = next_block(it))
		printf("%d\n", it->size);
	printf("\n");
	
	for(it = freeptr; it != NULL; it = next_block(it))
		printf("%d\n", it->size);
	printf("\n");
	
	return 0;
} 
