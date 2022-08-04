/**
 *
 *  Authors :           James Grant & Callum Anderson
 *  Last Modified :     8/12/19
 *  Version :           1.4
 *  Description :       Part 1 implements a memory manager by using a first fit allocation algorithm which loops over
 *                      a doubly linked list until finding the first free node, or NULL (at the end). We initialise our
 *                      memory manager by passing the initialise function a memory address to a heap which size is also
 *                      passed. Then a node struct is allocated at the head of the memory block (taking up 16 bytes)
 *                      with pointers to the next and previous node as well as the size of the block itself. Part 1 also
 *                      implements a deallocate function which takes a memory pointer to a block of allocated memory,
 *                      which can then be used to find the node at the head of the block and set it to free and unlink
 *                      it from the list thus deallocating the node. Finally, part 1 implements a printf function for
 *                      the memory manager which prints out the entire list in a readable format.
 *
 */

#include "part1.h"

Node *firstBlock; // Initialise pointer to first node of list

/**
 * Initialises first node which is a hole that takes up the entire heap. The node structure is then given information
 * about it's self. Null checks are conducted to make sure that memory allocation has worked.
 * Once the first node is made the first and last pointers are initialised.
 *
 * @param memory - pointer to heap
 * @param size - size of heap in bytes
 */
void initialise(void *memory , size_t size)
{
    Node *node = (Node *)(memory); // Assign struct to start of heap

    if (size == 0 || node == NULL)
    {
        fprintf(stderr, "Error: Unable to allocate memory in initialise().\n");
        exit(EXIT_FAILURE);
    }

    node->free = true;
    node->size = size - sizeof(Node); // Initialises size to not include the size of the struct
    node->next = NULL;
    node->prev = NULL;

    firstBlock = node;
}

/**
 * Allocates memory in the heap to a given process. This process is given a pointer to a block of memory with its
 * requested size. The function allocates memory by looking for the first free node with enough space to hold a new node
 * and a struct,even if the node is too big. Once a node has been found, its details are changed and a new free node is
 * created to fill the hole created.
 *
 * @param bytes - size of requested node to be allocated
 * @return - pointer to memory location
 */
void *allocate(size_t bytes)
{
    size_t totalBytes = bytes + sizeof(Node); // total size of a new node + memory block

    if (bytes < 1) return NULL;

    for (Node *node = firstBlock; node != NULL; node = node->next)
    {
        if (node->free == false || node->size < bytes) continue;  // Don't use non free nodes or too small nodes
        /* If block found has exact size or is exact size of memory + size of node allocate */
        if (node->size <= totalBytes)
        {
            node->free = false;
            return ((void *)(node) + sizeof(Node));
        }
        if (node->size > totalBytes)
        {
            Node *freeNode = (Node *)(((void *)(node) + sizeof(Node)) + bytes); // New empty node

            freeNode->free = true;
            freeNode->size = node->size - totalBytes;
            freeNode->prev = node;
            freeNode->next = node->next;

            if (freeNode->next != NULL) node->next->prev = freeNode; // Preserve list links

            node->free = false;
            node->size = bytes;
            node->next = freeNode;

            return ((void *)(node) + sizeof(Node));
        }
    }
    return NULL;
}

/**
 * Deallocate memory by setting the node->free to true so that it can be used in allocating. If there is a free node
 * before or after it, said node, is disconnected and the current node is then grown into it creating one big node.
 *
 * @param memory - the memory pointer to be unallocated
 */
void deallocate(void *memory)
{
    Node *node = (Node *)(memory); // Get node pointer from memory pointer
    if(node == NULL) return; // Make sure that the input is a valid pointer
    node --; // Moves back one node struct to the actual node struct

    Node *prevNode = node->prev;
    Node *nextNode = node->next;

    node->free = true;

    /* If next node can be coalesced */
    if(nextNode != NULL && nextNode->free == true)
    {
        node->next = nextNode->next;
        node->size += nextNode->size + sizeof(Node); // Increase main node size

        if (nextNode->next != NULL) nextNode->next->prev = node;
    }
    /* If previous node can be coalesced */
    if(prevNode != NULL && prevNode->free == true)
    {
        prevNode->next = node->next; // Un-link old node
        prevNode->size += node->size + sizeof(Node); // Increase main node size

        if (node->next != NULL) node->next->prev = prevNode;
    }
}

/**
 *  Prints out all node details in a readable format
 */
void memoryManager_printf()
{
    printf("\n");
    int blkCounter = 1; // Block counter
    for (Node *node = firstBlock; node != NULL; node = node->next)
    {
        // If at end don't print comma
        printf("Block : %d ",blkCounter++);
        printf(node->next == NULL ? "(Free : %d, Size : %d, Node Size : %d, Memory : %p)\n" : "(Free : %d, Size : %d, Node Size : %d, Memory : %p),\n",
                node->free, node->size, sizeof(Node), ((void *)(node) + sizeof(Node)));
    }
    printf("\n");
}