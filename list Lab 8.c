// list/list.c
//
// Implementation for linked list.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

// Declare node_free before its usage
void node_free(node_t *node);

list_t *list_alloc() {
    list_t *list = (list_t *)malloc(sizeof(list_t));
    list->head = NULL;
    return list;
}

node_t *node_alloc(block_t *blk) {
    node_t *node = malloc(sizeof(node_t));
    node->next = NULL;
    node->blk = blk;
    return node;
}

void list_free(list_t *l) {
    node_t *current = l->head;
    while (current) {
        node_t *next = current->next;
        node_free(current); // Correct usage of node_free
        current = next;
    }
    free(l);
}

void node_free(node_t *node) {
    free(node);
}

void list_print(list_t *l) {
    node_t *current = l->head;
    block_t *b;

    if (current == NULL) {
        printf("list is empty\n");
        return;
    }
    while (current != NULL) {
        b = current->blk;
        printf("PID=%d START:%d END:%d\n", b->pid, b->start, b->end);
        current = current->next;
    }
}

int list_length(list_t *l) {
    node_t *current = l->head;
    int i = 0;
    while (current != NULL) {
        i++;
        current = current->next;
    }
    return i;
}

void list_add_to_back(list_t *l, block_t *blk) {
    node_t *newNode = node_alloc(blk);
    newNode->next = NULL;
    if (l->head == NULL) {
        l->head = newNode;
    } else {
        node_t *current = l->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}

void list_add_to_front(list_t *l, block_t *blk) {
    node_t *newNode = node_alloc(blk);
    newNode->next = l->head;
    l->head = newNode;
}

void list_add_at_index(list_t *l, block_t *blk, int index) {
    int i = 0;

    node_t *newNode = node_alloc(blk);
    node_t *current = l->head;

    if (index == 0) {
        newNode->next = l->head->next;
        l->head = newNode;
    } else if (index > 0) {
        while (i < index && current->next != NULL) {
            current = current->next;
            i++;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

block_t *list_remove(list_t *l, block_t *blk) {
    if (!l || !blk || !l->head) {
        return NULL; // If the list, block, or head is NULL, return NULL
    }

    node_t *current = l->head;
    node_t *prev = NULL;

    while (current) {
        if (compareBlks(current->blk, blk)) { // If the current block matches
            if (prev) {
                prev->next = current->next; // Bypass the current node
            } else {
                l->head = current->next; // Update head if removing the first node
            }

            block_t *removed_blk = current->blk; // Store the block to return
            node_free(current); // Free the current node
            return removed_blk; // Return the removed block
        }
        prev = current; // Move the prev pointer
        current = current->next; // Move to the next node
    }

    return NULL; // Block not found
}

void list_add_ascending_by_address(list_t *l, block_t *newblk) {
    node_t *newNode = node_alloc(newblk);
    if (!l->head || newblk->start < l->head->blk->start) {
        newNode->next = l->head;
        l->head = newNode;
        return;
    }

    node_t *current = l->head;
    while (current->next && current->next->blk->start < newblk->start) {
        current = current->next;
    }

    newNode->next = current->next;
    current->next = newNode;
}

void list_add_ascending_by_blocksize(list_t *l, block_t *newblk) {
    node_t *newNode = node_alloc(newblk);
    int newblk_size = newblk->end - newblk->start + 1;

    if (!l->head || (newblk_size < (l->head->blk->end - l->head->blk->start + 1))) {
        newNode->next = l->head;
        l->head = newNode;
        return;
    }

    node_t *current = l->head;
    while (current->next &&
           (newblk_size >= (current->next->blk->end - current->next->blk->start + 1))) {
        current = current->next;
    }

    newNode->next = current->next;
    current->next = newNode;
}

block_t *list_remove_from_front(list_t *l) {
    if (!l->head) {
        return NULL;
    }

    node_t *current = l->head;
    block_t *blk = current->blk;
    l->head = current->next;
    node_free(current);

    return blk;
}

void list_coalesce_nodes(list_t *l) {
    if (!l->head || !l->head->next) {
        return; // Nothing to coalesce
    }

    node_t *prev = l->head;
    node_t *current = prev->next;

    while (current) {
        // Merge adjacent blocks
        if (prev->blk->end + 1 == current->blk->start) {
            prev->blk->end = current->blk->end; // Extend the block
            prev->next = current->next;        // Remove current node
            node_free(current);
            current = prev->next; // Move to the next node
        } else {
            prev = current;
            current = current->next;
        }
    }
}

// Compare Blocks Implementation
bool compareBlks(block_t *a, block_t *b) {
    if (!a || !b) {
        return false; // Handle null blocks
    }
    return (a->pid == b->pid) && (a->start == b->start) && (a->end == b->end);
}
