/**
 *
 *  Authors :           James Grant & Callum Anderson
 *  Last Modified :     8/12/19
 *  Version :           1.4
 *  Description :       Part 3 test file used to test the functionality of all the different algorithms and that the
 *                      memory manager is thread safe as specified.
 *
 */

#include "part3.h"

pthread_t threads[20];

/**
 * Function to check over the entire memory list to ensure that it is allocated properly and all addresses are
 * maintained.
 *
 * @param node - node pointer to first node
 * @param startAddress - address of first memory block
 * @return - true/false
 */
bool_type listTest(Node* node, void *startAddress)
{
    void *currentAddress = startAddress;
    size_t totalSize = 0;
    Node *firstNode = node;

    do{
        if (((void *)(node) + sizeof(Node)) != currentAddress) return false; // Checks address integrity

        currentAddress += (node->size + sizeof(Node)); // Increments address correctly
        totalSize += node->size + sizeof(Node); // Increments size
        node = node->next;
    }while(node != firstNode);
    if (totalSize != 200) return false; // Checks size of list integrity
    return true;
}

/**
 * Function that can be used to test the functionality of all the algorithms using threads.
 *
 * @param algorithm - algorithm to be used by the test memory manager
 */
void threadTest(char *algorithm)
{
    void *returnValue; // Variable used to store the return values for each thread
    printf("---------- Initialising Memory Manager ----------\n");
    printf("\n -- Starting memory list :\n");

    size_t size = 200;
    void *heap = malloc(size);
    initialise(heap, size, algorithm);

    Node *node = (Node *)(heap);
    void *startAddress = heap + sizeof(Node);
    int error;

    memoryManager_printf();

    printf("---------- Allocating Test ----------\n");
    printf("Creating multiple threads and allocating in parallel to test that part 3 is thread safe.\n");

    for (int i = 0; i < 20; i++)
    {
        error = pthread_create(&(threads[i]), NULL, (void *(*)(void *)) allocate, (void *) 10 + i);

        /* Check that thread has been successfully created, otherwise throw error */
        if (error != 0) fprintf(stderr, "Error: Unable to create thread in baseTest().\n");
    }

    printf("Allocating test : ");
    if (listTest(node, startAddress) == true) printf("Passed!\n");
    else printf("Failed!\n");

    memoryManager_printf();

    printf("---------- Deallocating Test ----------\n");
    printf("Create multiple threads and deallocate in parallel to test that part 3 is thread safe.\n");

    for (int i = 0; i < 20; i++)
    {
        pthread_join(threads[i], &returnValue); // Get the return values for each thread and print them
        printf(i == 19 ? "(Thread %d: %p)\n" : "(Thread %d: %p), ", threads[i], returnValue);

        error = pthread_create(&(threads[i]), NULL, (void *(*)(void *)) deallocate, returnValue); // Deallocate the blocks

        /* Check that thread has been successfully created, otherwise throw error */
        if (error != 0) fprintf(stderr, "Error: Unable to create thread in baseTest().\n");
    }

    printf("Deallocating test : ");
    if (node->free == true && node->size == 184) printf("Passed!\n");
    else printf("Failed!\n");

    memoryManager_printf();

    printf("---------- Lock Test ----------\n");
    printf("Try and fully allocate the memory to test that all locks have been unlocked.\n");

    void *test = allocate(184);
    printf("Lock Test : ");
    node = ((Node *)(test - sizeof(Node)));
    if (node->free == false && node->size == 184) printf("Passed!\n");
    else printf("Failed!\n");

    memoryManager_printf();
    free(heap);
}

/**
 * Function that tests each algorithm individually.
 */
void testHarness()
{
    printf("---------- Testing Begins ----------\n");

    printf(" -- As all the algorithms are based off of part 2, the only test necessary for them is that their "
           "algorithm operates correctly with threads -- \n ");

    printf("\n---------- Begin First Fit Test ----------\n");
    threadTest("FirstFit");
    printf("\n---------- End First Fit Test ----------\n");

    printf("\n---------- Begin Best Fit Test ----------\n");
    threadTest("BestFit");
    printf("\n---------- End Best Fit Test ----------\n");

    printf("\n---------- Begin Worst Fit Test ----------\n");
    threadTest("WorstFit");
    printf("\n---------- End Worst Fit Test ----------\n");

    printf("\n---------- Begin Next Fit Test ----------\n");
    threadTest("NextFit");
    printf("\n---------- End Next Fit Test ----------\n");

    printf("\n---------- Testing Ends ----------");
}

int main()
{
    testHarness();
    return EXIT_SUCCESS;
}