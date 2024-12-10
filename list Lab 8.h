#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

// Structure representing a memory block
typedef struct block {
    int pid;   // Process ID
    int start; // Start address of the memory partition
    int end;   // End address of the memory partition
} block_t;

// Structure representing a node in the linked list
typedef struct node {
    block_t *blk;      // Pointer to the block
    struct node *next; // Pointer to the next node
} node_t;

// Structure representing the linked list itself
typedef struct list {
    node_t *head; // Pointer to the first node in the list
} list_t;

// Function prototypes for managing the linked list

// Allocates memory for a new list and returns a pointer to it
list_t *list_alloc();

// Allocates memory for a new node containing a block and returns a pointer to it
node_t *node_alloc(block_t *blk);

// Frees all memory associated with the list
void list_free(list_t *l);

// Prints the contents of the list in a readable format
void list_print(list_t *l);

// Returns the length of the list
int list_length(list_t *l);

// Methods for adding blocks to the list
void list_add_to_back(list_t *l, block_t *blk);
void list_add_to_front(list_t *l, block_t *blk);
void list_add_at_index(list_t *l, block_t *blk, int index);
void list_add_ascending_by_address(list_t *l, block_t *blk);
void list_add_ascending_by_blocksize(list_t *l, block_t *blk);
void list_add_descending_by_blocksize(list_t *l, block_t *blk);

// Methods for removing blocks from the list
block_t* list_remove_from_back(list_t *l);
block_t* list_remove_from_front(list_t *l);
block_t* list_remove_at_index(list_t *l, int index);
block_t* list_remove(list_t *l, block_t *blk); // <--- Add it here!

// Checks if a specific block exists in the list by its pointer
bool list_is_in(list_t *l, block_t *blk);

// Checks if a block of a specific size exists in the list
bool list_is_in_by_size(list_t *l, int size);

// Checks if a block with a specific PID exists in the list
bool list_is_in_by_pid(list_t *l, int pid);

// Returns the element at a specific index in the list
block_t* list_get_elem_at(list_t *l, int index);

// Returns the index of a given block in the list
int list_get_index_of(list_t *l, block_t *blk);

// Returns the index of a block by its size in the list
int list_get_index_of_by_size(list_t *l, int size);

// Returns the index of a block by its PID in the list
int list_get_index_of_by_pid(list_t *l, int pid);

// Returns the front element of the list or NULL if empty
block_t* list_get_from_front(list_t *l);

// Compares two blocks for equality
bool compareBlks(block_t* a, block_t* b);

// Compares the PID of a block with a given PID
bool comparePID(int pid, block_t *blk);

// Compares the size of a block with a given size
bool compareSize(int size, block_t *blk);

// Joins adjacent nodes whose blocks are physically next to each other
void list_coalesce_nodes(list_t *l);

#endif // LIST_H
