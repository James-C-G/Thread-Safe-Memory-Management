/**
 *
 *  Authors :           James Grant & Callum Anderson
 *  Last Modified :     8/12/19
 *  Version :           1.4
 *  Description :       Part 1 test file used to test that both the allocate and deallocate function both work as
 *                      intended and allocate/don't allocate memory properly in all possible cases.
 *
 */

#include "part1.h"

/**
 * Test harness to test functionality of the memory manager.
 */
void testHarness()
{
    printf("---------- Testing Begins ----------\n\n");

    Node *node;
    size_t baseSize = 20;
    size_t failSize = 2000;
    size_t nodeSize = sizeof(Node);

    printf("---------- Initialising Memory Manager ----------\n");
    size_t heapSize = 200;
    void *heap = malloc(heapSize);
    initialise(heap, heapSize);
    memoryManager_printf(); // prints one node with size of 184 as sizeof(Node) = 16

    printf("---------- Allocate Test ----------\n");
    printf("Over allocate test : ");
    /* Checking that when allocating a size too big, it returns NULL */
    if (allocate(failSize) == NULL) printf("Passed!\n");
    else printf("Failed!\n");

    void *test1 = allocate(baseSize);
    node = (Node *)(test1 - sizeof(Node)); // Setting up node pointer to allocated node

    printf("Allocate test : ");
    /* Checking that the node is not free and has a size that stores the requested amount of bytes */
    if (node->free == false && node->size >= baseSize) printf("Passed!\n");
    else printf("Failed!\n");
    memoryManager_printf();

    printf("---------- Deallocate Test ----------\n");
    printf("Single deallocate test : ");
    deallocate(test1);
    node = (Node *)(test1 - sizeof(Node)); // Setting up node pointer to deallocated node

    /* Checking that when the node is deallocated it is set to free */
    if(node->free == true) printf("Passed!\n");
    else printf("Failed!\n");
    memoryManager_printf(); // Shows coalescing back into one free node

    void *test2 = allocate(baseSize);
    void *test3 = allocate(baseSize);
    void *test4 = allocate(baseSize);
    void *test5 = allocate(baseSize);
    deallocate(test3);

    printf(" -- Triple Coalescing on Blocks 1, 2, and 3 \n");
    memoryManager_printf();

    printf("Triple coalescing deallocate test : ");

    deallocate(test4);
    deallocate(test2);
    node = (Node *)(test2 - sizeof(Node)); // Setting up node pointer to coalesced node

    /* Checking that the node is not free and that after coalescing its size has been increased to the correct size */
    if(node->free == true && node->size == ((baseSize * 3) + (nodeSize * 2))) printf("Passed!\n");
    else printf("Failed!\n");
    memoryManager_printf(); // Shows coalescing back into one free node
    deallocate(test5); // Emptying list

    printf("---------- No Room For New Node Test ----------\n");
    printf(" -- If a node has a size <= the amount of memory requested plus the size of a node then the node must be given"
           " even though it has more room than needed.\n");
    memoryManager_printf();

    void *test7 = allocate(180); // Allocating just under maximum amount as total space is (200 - 16) 184;
    node = (Node *)(test7 - sizeof(Node)); // Setting up node pointer for allocated node
    printf("No room test : ");

    /* Checking that when allocating to a node that doesn't have enough room to create a new node that it is used anyway */
    if(node->free == false && node->size == (heapSize - sizeof(Node))) printf("Passed!\n");
    else printf("Failed!\n");

    memoryManager_printf();

    deallocate(test7);

    printf("---------- Repeated Allocation Test ----------\n");

    /* Checking that the list can be repeatedly allocated and not allocate after being filled */
    while (allocate(20) != NULL) printf("Repeated allocation test allocated successfully.\n");
    printf("Repeated allocation test returned NULL.\n");

    free(heap);

    printf("\n---------- Testing Ends ----------");
}

int main()
{
    testHarness();
    return EXIT_SUCCESS;
}