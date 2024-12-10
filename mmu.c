#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "list.h"
#include "util.h"

void TOUPPER(char *arr) {
    for (int i = 0; i < strlen(arr); i++) {
        arr[i] = toupper(arr[i]);
    }
}

void get_input(char *args[], int input[][2], int *n, int *size, int *policy) {
    FILE *input_file = fopen(args[1], "r");
    if (!input_file) {
        fprintf(stderr, "Error: Invalid filepath\n");
        exit(0);
    }

    parse_file(input_file, input, n, size);
    fclose(input_file);

    TOUPPER(args[2]);
    if (strcmp(args[2], "-F") == 0 || strcmp(args[2], "-FIFO") == 0)
        *policy = 1;
    else if (strcmp(args[2], "-B") == 0 || strcmp(args[2], "-BESTFIT") == 0)
        *policy = 2;
    else if (strcmp(args[2], "-W") == 0 || strcmp(args[2], "-WORSTFIT") == 0)
        *policy = 3;
    else {
        printf("usage: ./mmu <input file> -{F | B | W }  \n(F=FIFO | B=BESTFIT | W=WORSTFIT)\n");
        exit(1);
    }
}

void allocate_memory(list_t *freelist, list_t *alloclist, int pid, int blocksize, int policy) {
    block_t *chosen = NULL;
    node_t *current = freelist->head;

    // Traverse the free list to find a suitable block
    while (current) {
        block_t *blk = current->blk;
        int blk_size = blk->end - blk->start + 1;

        if (blk_size >= blocksize) {
            if (policy == 1) { // FIFO
                chosen = blk;
                break;
            } else if (policy == 2) { // BESTFIT
                if (!chosen || blk_size < (chosen->end - chosen->start + 1))
                    chosen = blk;
            } else if (policy == 3) { // WORSTFIT
                if (!chosen || blk_size > (chosen->end - chosen->start + 1))
                    chosen = blk;
            }
        }
        current = current->next;
    }

    if (!chosen) {
        printf("Error: Not Enough Memory for %d blocks\n", blocksize);
        return;
    }

    // Allocate memory and fragment the block
    block_t *allocated_blk = list_remove(freelist, chosen);
    allocated_blk->pid = pid;
    allocated_blk->end = allocated_blk->start + blocksize - 1;

    list_add_ascending_by_address(alloclist, allocated_blk);

    // Create a fragmented block if there's remaining space
    if (allocated_blk->end < chosen->end) {
        block_t *fragment = malloc(sizeof(block_t));
        fragment->pid = 0; // Free block
        fragment->start = allocated_blk->end + 1;
        fragment->end = chosen->end;
        list_add_ascending_by_address(freelist, fragment);
    }
}

void deallocate_memory(list_t *alloclist, list_t *freelist, int pid, int policy) {
    node_t *current = alloclist->head;

    while (current) {
        block_t *blk = current->blk;

        if (blk->pid == pid) {
            blk = list_remove(alloclist, blk);
            blk->pid = 0; // Mark block as free
            list_add_ascending_by_address(freelist, blk); // Add by address first
            list_coalesce_nodes(freelist); // Coalesce free blocks

            if (policy == 2 || policy == 3) { // BESTFIT or WORSTFIT
                // Optionally, sort the freelist by block size
                // This requires an additional function to reorder the list.
            }

            printf("Deallocated Memory for PID: %d\n", pid);
            return;
        }
        current = current->next;
    }

    printf("Error: Can't locate Memory Used by PID: %d\n", pid);
}


list_t* coalese_memory(list_t *list) {
    list_t *temp_list = list_alloc();
    block_t *blk;

    while ((blk = list_remove_from_front(list)) != NULL) {
        list_add_ascending_by_address(temp_list, blk);
    }

    list_coalesce_nodes(temp_list); // Coalesce adjacent blocks

    list_free(list); // Free the old list
    return temp_list; // Return the coalesced list
}

void print_list(list_t *list, char *message) {
    node_t *current = list->head;

    printf("%s:\n", message);

    while (current != NULL) {
        block_t *blk = current->blk;
        printf("Block:\t START: %d\t END: %d", blk->start, blk->end);

        if (blk->pid != 0)
            printf("\t PID: %d\n", blk->pid);
        else
            printf("\n");

        current = current->next;
    }
}

int main(int argc, char *argv[]) {
    int PARTITION_SIZE, inputdata[200][2], N = 0, Memory_Mgt_Policy;

    list_t *FREE_LIST = list_alloc();
    list_t *ALLOC_LIST = list_alloc();

    if (argc != 3) {
        printf("usage: ./mmu <input file> -{F | B | W }  \n(F=FIFO | B=BESTFIT | W=WORSTFIT)\n");
        exit(1);
    }

    get_input(argv, inputdata, &N, &PARTITION_SIZE, &Memory_Mgt_Policy);

    block_t *partition = malloc(sizeof(block_t));
    partition->start = 0;
    partition->end = PARTITION_SIZE - 1;

    list_add_to_front(FREE_LIST, partition);

    for (int i = 0; i < N; i++) {
        printf("************************\n");

        if (inputdata[i][0] != -99999 && inputdata[i][0] > 0) {
            printf("ALLOCATE: %d FROM PID: %d\n", inputdata[i][1], inputdata[i][0]);
            allocate_memory(FREE_LIST, ALLOC_LIST, inputdata[i][0], inputdata[i][1], Memory_Mgt_Policy);
        } else if (inputdata[i][0] != -99999 && inputdata[i][0] < 0) {
            printf("DEALLOCATE MEM: PID %d\n", abs(inputdata[i][0]));
            deallocate_memory(ALLOC_LIST, FREE_LIST, abs(inputdata[i][0]), Memory_Mgt_Policy);
        } else {
            printf("COALESCE/COMPACT\n");
            FREE_LIST = coalese_memory(FREE_LIST);
        }

        printf("************************\n");

        print_list(FREE_LIST, "Free Memory");
        print_list(ALLOC_LIST, "\nAllocated Memory");

        printf("\n\n");
    }

    list_free(FREE_LIST);
    list_free(ALLOC_LIST);

    return 0;
}
