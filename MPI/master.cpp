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
#include "mpiMsgTag.h"

int totalProcesses = 0;

/**
 * @brief occupy a worker
 *
 * calls a worker and gives him something to do
 *
 * @param[in] the baseString, that will be checked by a worker
 */
void CallMPIProcess(const string baseStringPwd)
{
    static int currentProcess=0;
    if(currentProcess == MasterProcess)
    {
        currentProcess++;
    }

#ifdef VERBOSE
    cout << "calling process " << currentProcess << " of " << totalProcesses-1 << endl;
#endif // VERBOSE

    // ...evil const_cast...
    MPI_Send(const_cast<char*>(baseStringPwd.c_str()), baseStringPwd.length(), MPI_BYTE, currentProcess, task, MPI_COMM_WORLD);//, &request[currentProcess]);

    currentProcess++;
    if(currentProcess >= totalProcesses)
    {
        currentProcess=0;
    }
}

/**
 * @brief recursive implementation of bruteforce
 *
 * generates all baseString with 1,2,3,...,width-1 characters
 * and tells a worker the check it
 * call it as follows: bruteRecursive(string(""), width);
 *
 * @param[in]   baseString: a const string indicates the prefix of a string to be checked
 * @param[in]   minWidth:   all baseStrings shorter than minWidth will not be checked
 * @param[in]   maxWidth:   the maximum length of baseString you wish to be checked
 */
void bruteRecursive(const string baseString, const unsigned short &minWidth, const unsigned short &maxWidth)
{
    if(baseString.length()+1 > minWidth)
    {
        CallMPIProcess(baseString);
    }

    for(int i=0; i<SizeAlphabet; i++)
    {
        if (baseString.length()+1 < maxWidth)
        {
            bruteRecursive(baseString+alphabet[i], minWidth, maxWidth);
        }
    }
}
