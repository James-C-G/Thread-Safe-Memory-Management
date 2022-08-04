/**
 *
 *  Authors :           James Grant & Callum Anderson
 *  Last Modified :     8/12/19
 *  Version :           1.4
 *  Description :       Part 1 header file that defines all structs and function names.
 *
 */

#ifndef COURSEWORK_2_PART1_H
#define COURSEWORK_2_PART1_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Enum that implements a make shift bool type that we can use to make code more readable.
 */
typedef enum
{
    true = 1,
    false = 0
}bool_type;

/**
 * Node struct that stores information about the current node in the memory manager linked list
 */
typedef struct _Node
{
    bool_type free;
    size_t size;
    struct _Node *next;
    struct _Node *prev;
}Node;

/* Function names */

void initialise(void *memory , size_t size);

void *allocate(size_t bytes);

void deallocate(void *memory);

void memoryManager_printf();

#endif //COURSEWORK_2_PART1_H

