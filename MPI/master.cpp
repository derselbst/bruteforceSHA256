/**
 * Author: derselbst
 * Description: provides functions that are used by the master process, which performs the bruteforce attack
 */

#include <iostream>
#include <cstring> // memcmp()
#include <string>
#include <queue>
#include <limits>
#include <openssl/sha.h>
#include <byteswap.h>
#include <mpi.h>

using namespace std;

#include "alphabet.h"
#include "master.h"

// used when sending messages
enum MpiMsgTag
{
    task,
    success // hashes match, unused ATM
};

int totalProcesses = 0;

/**
 * @brief occupy a worker
 * 
 * calls a worker and gives him something to do
 * 
 * @param[in] the string, that will be checked by a worker
 */
void CallMPIProcess(const string guessedPwd)
{
    static int currentProcess=0;
    if(currentProcess == MasterProcess)
    {
        currentProcess++;
    }

    // ...evil const_cast...
    MPI_Send(const_cast<char*>(guessedPwd.c_str()), guessedPwd.length(), MPI_BYTE, currentProcess, task, MPI_COMM_WORLD);//, &request[currentProcess]);

    currentProcess++;
    if(currentProcess >= totalProcesses)
    {
        currentProcess=0;
    }
}

/**
 * @brief iterative implementation of bruteforce
 *
 * generates all permutations of a string with 1, 2, 3, ..., width characters
 * and tells a worker the check it
 * 
 * call it as follows: bruteIterative(width);
 *
 * @param[in]   width:      the maximum number of characters you wish to be checked
 */
void bruteIterative(const unsigned int width)
{
    queue<string> myQueue;

    // myQueue must contain at least one element when entering loop
    // else: SIGSEGV
    // hence, start checking with an empty string
    myQueue.push("");

    do
    {
        string baseString = myQueue.front();
        myQueue.pop();

        for(int i=0; i<SizeAlphabet; i++)
        {
            if (baseString.length()+1 < width)
            {
                myQueue.push(baseString+alphabet[i]);
            }

            CallMPIProcess(baseString+alphabet[i]);
        }
    }
    while(!myQueue.empty());
}
