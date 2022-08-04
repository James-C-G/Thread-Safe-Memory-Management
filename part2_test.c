/**
 *
 *  Authors :           James Grant & Callum Anderson
 *  Last Modified :     8/12/19
 *  Version :           1.4
 *  Description :       Part 2 test file used to test the functionality of all the different algorithms and that the
 *                      new circular queue works as intended.
 *
 */

#include "part2.h"

/**
 * Function that test the functionality of the first fit algorithm
 */
void firstFitTest()
{
    Node *node;
    size_t baseSize = 20;
    size_t failSize = 2000;
    size_t nodeSize = sizeof(Node);

    printf("---------- Initialising Memory Manager ----------\n");
    size_t heapSize = 200;
    void *heap = malloc(heapSize);
    initialise(heap, heapSize, NULL);
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
}

/**
 * Function that test the functionality of the best fit algorithm
 */
void bestFitTest()
{
    printf("---------- Initialising Memory Manager ----------\n");
    printf("\n -- Starting memory list :\n");
    size_t size = 400;
    void *heap = malloc(size);
    initialise(heap, size, "BestFit");
    Node *node;

    void *test1 = allocate(60);
    void *test2 = allocate(40);
    void *test3 = allocate(30);
    void *test4 = allocate(5);
    void *test5 = allocate(30);
    memoryManager_printf();

    printf("---------- Best Fit Allocation Test ----------\n");
    printf("\n -- Deallocate blocks 2 and 4 so there are different options for the algorithm to choose.\n");

    deallocate(test2);
    deallocate(test4);
    memoryManager_printf();

    printf(" -- Allocating 5 bytes should fill the hole of 5 bytes and ignore all other processes.\n");
    node = (Node *)(test4 - sizeof(Node)); // Setting up node pointer to allocated node

    allocate(5);
    memoryManager_printf();

    printf("Best fit allocation test : ");

    /* Checking that the correct node has been allocated to */
    if(node->free == false && node->size >= 5) printf("Passed!\n");
    else printf("Failed!\n");

    free(heap);
}

/**
 * Function that test the functionality of the worst fit algorithm
 */
void worstFitTest()
{
    printf("---------- Initialising Memory Manager ----------\n");
    printf("\n -- Starting memory list :\n");
    size_t size = 200;
    void *heap = malloc(size);
    initialise(heap, size, "WorstFit");
    Node *node;

    void *test1 = allocate(20);
    void *test2 = allocate(5);
    void *test3 = allocate(30);
    void *test4 = allocate(5);
    void *test5 = allocate(60);
    memoryManager_printf();

    printf("---------- Worst Fit Allocation Test ----------\n");
    printf("\n -- Deallocate blocks 2 and 4 so there are different options for the algorithm to choose.\n");

    deallocate(test2);
    deallocate(test5);
    memoryManager_printf();

    printf(" -- Allocating 5 bytes should fill the end hole, which should have a size of 60 bytes, and create a new node to "
           "fill the space ignoring all other processes.\n");
    node = (Node *)(test5 - sizeof(Node)); // Setting up node pointer to allocated node

    allocate(5);
    memoryManager_printf();

    printf("Worst fit allocation test : ");

    /* Checking that the correct node has been allocated to */
    if(node->free == false && node->size >= 5) printf("Passed!\n");
    else printf("Failed!\n");

    free(heap);
}

/**
 * Function that test the functionality of the next fit algorithm
 */
void nextFitTest()
{
    printf("---------- Initialising Memory Manager ----------\n");
    printf("\n -- Starting memory list :\n");
    size_t size = 240;
    void *heap = malloc(size);
    initialise(heap, size, "NextFit");

    void *test1 = allocate(20);
    void *test2 = allocate(10); // Smaller so that it is left out of the check
    void *test3 = allocate(20);
    void *test4 = allocate(20);
    void *test5 = allocate(20);
    void *test6 = allocate(54); // Set the allocation to 6

    deallocate(test2);
    memoryManager_printf();

    Node *node = (Node *)(test3 - sizeof(Node)); // Setting up node pointer to allocated block

    printf("---------- Next Fit Allocation Test ----------\n");

    printf(" -- After allocating smaller memory into block 2, the next fit algorithm should start its search form there. "
           "This means that if I deallocate blocks 1 and 3, and then try and allocate the algorithm should fill block "
           "3 rather than block 1.\n");

    void *test7 = allocate(10); // allocates in block 2
    deallocate(test1);
    deallocate(test3);
    memoryManager_printf();

    printf("Next fit allocation test : ");
    void *test9 = allocate(20); // allocates in block 3

    /* Checks that the node is not empty and has been correctly assigned */
    if (node->free == false && node->size == 20) printf("Passed!\n");
    else printf("Failed!\n");

    memoryManager_printf();

    printf("---------- Next Fit Deallocation Preservation Test ----------\n");

    printf(" -- After resetting the list and deallocating necessary processes we have the last used set to block 4. This "
           "means that when we deallocate it, it should merge blocks 3, 4, and 5 all into one, preserving the last used "
           "and then when allocating again the block they all merge to should be used.\n");

    /* Resetting the list */
    deallocate(test6);
    void *test10 = allocate(20); // Allocates block 6
    void *test11 = allocate(18); // Allocates block 7
    void *test12 = allocate(20); // Allocates block 1
    deallocate(test4); // Deallocate block 4
    void *test13 = allocate(20); // Allocate block 4
    deallocate(test12); // Deallocate block 1
    deallocate(test9); // Deallocate block 3
    deallocate(test5); // Deallocate block 5
    deallocate(test11); // Deallocate block 7
    memoryManager_printf();

    /* Testing */
    node = (Node *)(test9 - sizeof(Node)); // Setting up node pointer to merged block
    deallocate(test13); // Deallocate block 4
    void *test14 = allocate(20);

    printf("Next fit deallocation preservation test : ");
    if (node->free == false && node->size == 20) printf("Passed!\n");
    else printf("Failed!\n");

    memoryManager_printf();
    free(heap);
}

/**
 * Test harness to test functionality of the memory manager.
 */
void testHarness()
{
    printf("---------- Testing Begins ----------\n");

    printf(" -- As all the algorithms are based off of first fit, the only test necessary for them is that their "
           "algorithm operates correctly as all other edge cases are the same as the ones tested in the first "
           "fit test -- \n");

    printf("\n---------- Begin First Fit Test ----------\n");
    firstFitTest();
    printf("\n---------- End First Fit Test ----------\n");

    printf("\n---------- Begin Best Fit Test ----------\n");
    bestFitTest();
    printf("\n---------- End Best Fit Test ----------\n");

    printf("\n---------- Begin Worst Fit Test ----------\n");
    worstFitTest();
    printf("\n---------- End Worst Fit Test ----------\n");

    printf("\n---------- Begin Next Fit Test ----------\n");
    nextFitTest();
    printf("\n---------- End Next Fit Test ----------\n");

    printf("\n---------- Testing Ends ----------");
}

int main()
{
    testHarness();
    return EXIT_SUCCESS;
}