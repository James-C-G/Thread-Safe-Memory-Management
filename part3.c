/**
 *
 *  Authors :           James Grant & Callum Anderson
 *  Last Modified :     8/12/19
 *  Version :           1.4
 *  Description :       Part 3 improves upon part 2 by implementing the means to allow the program to be ran across
 *                      multiple threads using a shared pool of memory it achieves this by using mutual exclusion and
 *                      locking.
 *
 */

#include "part3.h"

pthread_mutex_t lock;

Node *firstBlock; // Initialise pointer to first node of list
Node *lastUsed; // Initialise pointer that points to last accessed node (specific to nextFit)

/**
 * A function pointer that allocates memory in the heap to a given process. This process is given a pointer to a block
 * of memory with its requested size.
 *
 * @param - amount of requested bytes
 * @return - void pointer to a memory location
 */
void* (*allocate)(size_t);

/**
 * This support function is used to reduce code duplication as it takes out the common function of creating a new node
 * when the current node is shrunk and set to being used. After the current node is shrunk, a new node is created to
 * fill the rest of the space and is set to free.
 *
 * @param node - current node to be shrunk down to size
 * @param totalBytes - the total amount of memory required (bytes plus size of a Node structure)
 * @param bytes - the amount of memory to be allocated
 * @return - the edited current node
 */
Node *freeNode(Node *node, size_t bytes)
{
    size_t totalBytes = bytes + sizeof(Node);
    Node *freeNode = (Node *) (((void *)(node) + sizeof(Node)) + bytes); // New empty node

    freeNode->free = true;
    freeNode->size = node->size - totalBytes;
    freeNode->prev = node;
    freeNode->next = node->next;

    if (freeNode->next != NULL) node->next->prev = freeNode; // Preserve list links

    node->free = false;
    node->size = bytes;
    node->next = freeNode;

    return node;
}

/**
 * The algorithm allocates memory by looking for the first free node with enough space to hold a new node and a struct,
 * even if the node is too big. Once a node has been found, its details are changed and a new free node is created if a
 * hole is created. This function also locks the current thread when accessing the main pool of memory and unlocks it
 * when returning the memory address.
 *
 * @param bytes - requested bytes to be allocated
 * @return - void memory pointer/NULL if can't be allocated
 */
void *firstFit(size_t bytes)
{
    size_t totalBytes = bytes + sizeof(Node); // total size of a new node + memory block

    if (bytes < 1) return NULL;

    pthread_mutex_lock(&lock);
    Node *node = firstBlock->prev;

    do
    {
        node = node->next; // Increment through the list
        if (node->free == false || node->size < bytes) continue;  // Don't use non free nodes or too small nodes

        /* If block found has exact size or is exact size of memory + size of node allocate */
        if (node->size <= totalBytes)
        {
            node->free = false;
            pthread_mutex_unlock(&lock);
            return (void *)((void *)(node) + sizeof(Node));
        }
        pthread_mutex_unlock(&lock);
        return (void *)((void *)freeNode(node, bytes)) + sizeof(Node);

    }while(node->next != firstBlock); // End of loop met

    pthread_mutex_unlock(&lock);
    return NULL;
}

/**
 * This algorithm is similar to first fit but rather than starting from the first block every time when looping, it
 * starts from the last node accessed. This means that after accessing/creating a new node, a variable - lastUsed - is
 * updated to update the starting point. This variable is set to the start block at the start of the program. Once a
 * node has been found, its details are changed and a new free node is created if a hole is created. This function also
 * locks the current thread when accessing the main pool of memory and unlocks it when returning the memory address.
 *
 * @param bytes - requested bytes to be allocated
 * @return - void memory pointer/NULL if can't be allocated
 */
void *nextFit(size_t bytes)
{
    size_t totalBytes = bytes + sizeof(Node); // total size of a new node + memory block

    if (bytes < 1) return NULL;

    pthread_mutex_lock(&lock);
    /* lastUsed is used for readability but isn't necessary as firstBlock could just be continually updated for it to
     * also work as intended. */
    Node *node = lastUsed->prev;

    do
    {
        node = node->next; // Increment through the list
        if (node->free == false || node->size < bytes) continue;  // Don't use non free nodes or too small nodes

        lastUsed = node; // Update last accessed node

        /* If block found has exact size or is exact size of memory + size of node allocate */
        if (node->size <= totalBytes)
        {
            node->free = false;
            pthread_mutex_unlock(&lock);
            return (void *)((void *) (node) + sizeof(Node));
        }

        pthread_mutex_unlock(&lock);
        return (void *)((void *)(freeNode(node, bytes)) + sizeof(Node));

    }while(node->next != lastUsed); // End of loop met

    pthread_mutex_unlock(&lock);
    return NULL;
}

/**
 * This algorithm allocates memory by looping over the entire list and finding the smallest sized node possible that
 * will fit the requested amount of bytes. This is saved in a node variable and is updated when a better option is
 * found. At the end of the loop the node is then either used if a new node can't be created or a new node is created
 * using the freeNode function. This function also locks the current thread when accessing the main pool of memory and
 * unlocks it when returning the memory address.
 *
 * @param bytes - requested bytes to be allocated
 * @return - void memory pointer/NULL if can't be allocated
 */
void *bestFit(size_t bytes)
{
    size_t totalBytes = bytes + sizeof(Node); // total size of a new node + memory block
    Node *bestNode = NULL;

    if (bytes < 1) return NULL;

    pthread_mutex_lock(&lock);
    Node *node = firstBlock->prev;

    do
    {
        node = node->next; // Increment through the list
        if (node->free == false || node->size < bytes) continue;  // Don't use non free nodes or too small nodes

        if (node->size == bytes)
        {
            node->free = false;
            pthread_mutex_unlock(&lock);
            return (void *)((void *) (node) + sizeof(Node));
        }
        if (bestNode == NULL || node->size < bestNode->size) bestNode = node;

    }while(node->next != firstBlock);

    if (bestNode != NULL)
    {
        bestNode->free = false; // If no new node can be created, use whole node
        if (bestNode->size > totalBytes) bestNode = freeNode(bestNode, bytes); // If new node can be created

        pthread_mutex_unlock(&lock);
        return ((void *)((void *)(bestNode) + sizeof(Node))); // End of loop met
    }
    pthread_mutex_unlock(&lock);
    return NULL;
}

/**
 * This algorithm allocates memory by looping over the entire list and finding the biggest sized node possible that will
 * fit the requested amount of bytes. This is saved in a node variable and is updated when a bigger option is found. At
 * the end of the loop the node is then either used if a new node can't be created or a new node is created using the
 * freeNode function. This function also locks the current thread when accessing the main pool of memory and unlocks it
 * when returning the memory address.
 *
 * @param bytes - requested bytes to be allocated
 * @return - void memory pointer/NULL if can't be allocated
 */
void *worstFit(size_t bytes)
{
    size_t totalBytes = bytes + sizeof(Node); // total size of a new node + memory block
    Node *worstNode = NULL;

    if (bytes < 1) return NULL;

    pthread_mutex_lock(&lock);
    Node *node = firstBlock->prev;

    do
    {
        node = node->next; // Increment through the list

        if (node->free == false || node->size < bytes) continue;  // Don't use non free nodes or too small nodes

        if (node->size >= bytes)
        {
            if (worstNode == NULL || node->size > worstNode->size) worstNode = node; // Updating worst node variable
        }

    }while(node->next != firstBlock);

    if (worstNode != NULL)
    {
        worstNode->free = false;
        if (worstNode->size > totalBytes) worstNode = freeNode(worstNode, bytes);

        pthread_mutex_unlock(&lock);
        return (void *)((void *)(worstNode) + sizeof(Node)); // End of loop met
    }
    pthread_mutex_unlock(&lock);
    return NULL;
}

/**
 * Initialises first node which is a hole that takes up the entire heap. The node structure is then given information
 * about it's self. Null checks are conducted to make sure that memory allocation has worked.
 * Once the first node is made the first and last pointers are initialised.
 * Also, using the algorithm parameter, the function pointer for allocate is created based on which algorithm is
 * passed - if an invalid one is chosen, first fit is chosen by default.
 *
 * @param memory - pointer to heap
 * @param size - size of heap in bytes
 * @param algorithm - the algorithm to be used
 */
void initialise(void *memory , size_t size, char *algorithm)
{
    if (algorithm == NULL) allocate = &firstFit; // Check for NULL being passed in, and default to firstFit
    else if (!strcmp(algorithm, "BestFit")) allocate = &bestFit;
    else if (!strcmp(algorithm, "WorstFit")) allocate = &worstFit;
    else if (!strcmp(algorithm, "NextFit")) allocate = &nextFit;
    else allocate = &firstFit; // If anything else, default to firstFit.

    Node *node = (Node *)(memory); // Assign struct to start of heap

    if (size == 0 || node == NULL)
    {
        fprintf(stderr, "Error: Unable to allocate memory in initialise().\n");
        exit(EXIT_FAILURE);
    }

    node->free = true;
    node->size = size - sizeof(Node); // Initialises size to not include the size of the struct
    node->next = node;
    node->prev = node;

    firstBlock = lastUsed = node; // Sets up lastUsed in all cases for readability

    lock = PTHREAD_MUTEX_INITIALIZER; // Initialise the lock with default behaviour
}

/**
 * Deallocate memory by setting the node->free to true so that it can be used in allocating. If there is a free node
 * before or after it, said node, is disconnected and the current node is then grown into it creating one big node.
 * Also, if nodes are being coalesced the lastUsed node is preserved if its node is unlinked.
 *
 * @param memory - the memory pointer to be unallocated
 */
void deallocate(void *memory)
{
    Node *node = (Node *)(memory); // Get node pointer from memory pointer
    if(node == NULL) return; // Make sure that the input is a valid pointer
    node --; // Moves back one node struct to the actual node struct

    pthread_mutex_lock(&lock);

    Node *prevNode = node->prev;
    Node *nextNode = node->next;

    node->free = true;

    /* If next node can be coalesced and prevent wrap coalescing (front & end joining) */
    if(nextNode != node && nextNode->free == true && nextNode != firstBlock)
    {
        /* If in next fit and coalesced node is last used, update last used to preserve */
        if (lastUsed == nextNode) lastUsed = node;

        node->next = nextNode->next;
        node->size += nextNode->size + sizeof(Node); // Increase main node size

        if (nextNode->next != node) nextNode->next->prev = node;
    }

    /* If previous node can be coalesced and prevent wrap coalescing (front & end joining) */
    if(prevNode != node && prevNode->free == true && node != firstBlock)
    {
        /* If in next fit and coalesced node is last used, update last used to preserve */
        if (lastUsed == node) lastUsed = prevNode;

        prevNode->next = node->next; // Un-link old node
        prevNode->size += node->size + sizeof(Node); // Increase main node size

        if (node->next != node) node->next->prev = prevNode;
    }
    pthread_mutex_unlock(&lock);
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
        printf(node->next == firstBlock ? "(Free : %d, Size : %d, Node Size : %d, Memory : %p)\n":"(Free : %d, Size : %d, Node Size : %d, Memory : %p),\n",
               node->free, node->size, sizeof(Node), ((void *)(node) + sizeof(Node)));
        if(node->next == firstBlock) break;
    }
    printf("\n");
}
