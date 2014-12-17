#include <conio.h>
#include <stdio.h>

#pragma region HEAP
#define HEAP_SIZE 2048
char HEAP[HEAP_SIZE];
int heap_head = 0;

void * sbrk(size_t size_incr){
	void *  tmp = &HEAP[heap_head];
	heap_head += size_incr;
	return tmp;
}

#pragma endregion

#pragma region HEAP METADATA

#define META_SIZE sizeof(struct heap_meta)
void * global_base = NULL;

struct heap_meta{
	size_t size;
	struct heap_meta * next;
	int free;
};

#pragma endregion

#pragma region MALLOC

struct heap_meta *find_free_block(struct heap_meta ** last, size_t size){
	struct heap_meta * curr = global_base; // START SEARCHING FROM THE START
	while (curr && !(curr->free && curr->size >= size)){
		*last = curr;
		curr = curr->next;
	}
	return curr;
}

struct heap_meta * request_space(struct heap_meta* last, size_t size){
	// THERE IS NO SPACE FREE IN THE HEAP TO FIND, REQUEST MORE:
	struct heap_meta * block;
	block = sbrk(size + META_SIZE);
	if (block == (void*) -1) return NULL;

	if (last) last->next = block;

	block->size = size;
	block->next = NULL;
	block->free = 0;
	return block;
}

void * malloc(size_t size){
	struct heap_meta * block;

	if (size <= 0) return NULL;

	if (!global_base){	// WILL HAPPEN ON FIRST RUN, THE HEAP SIZE IS EMPTY
		block = request_space(NULL, size);
		if (!block) return NULL;
		global_base = block; // THE NEW BASE IS THE NEW REQUESTED BLOCK
	} else { // HAPPENS AFTER ^ HAS RUN
		struct heap_meta * last = global_base;
		block = find_free_block(&last, size); // FIND FIT BLOCK ON THE HEAP
		if (!block){ // COULDN'T FIND FIT, REQUEST MORE SPACE
			block = request_space(last, size);
			if (!block) return NULL;
		} else block->free = 0; // FOUND FIT BLOCK!
	}
	return (block + 1);
}

#pragma endregion

#pragma region FREE
struct heap_meta *get_block_ptr(void * ptr){
	return (struct heap_meta*)ptr - 1;
}
void* free(void * ptr) {
	if (!ptr) return;

	struct heap_meta * block_ptr = get_block_ptr(ptr);
	block_ptr->free = 1;
	return NULL;
}
#pragma endregion

void dump_heap(int limit){
	for (int i = 0; i < limit; i++) printf("%d,%d = %d\t|%c|\n",&HEAP[i], i, HEAP[i], HEAP[i]);
}

int strlen(char*str) {
	int len = 0;
	for (; str[len] != '\0'; len++);
	return len;
}

void str(char* ptr, char* str){
	int len = strlen(str);
	while (len--) *(ptr++) = *(str++);
}

void main(){
	for (int i = 0; i < HEAP_SIZE; i++) HEAP[i] = '.'; // FILL WITH DOTS

	char * c = (char*) malloc(sizeof(char) * 2);
	char * c2 = (char*) malloc(sizeof(char) * 2);
	str(c,"adc"); // COPY STRING LITERAL TO PTR
	
	dump_heap(50);
	_getch();
}