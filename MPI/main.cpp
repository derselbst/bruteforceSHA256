/**
 * Author: derselbst
 * Description: a demo program
 */

#include <iostream>
#include <string>
#include <openssl/sha.h>
#include <mpi.h>

using namespace std;

#include "worker.h"
#include "master.h"

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    string pwd=">$<GV";

    // initialize the hash buffer for the password
    if(!generateSHA256(pwd.c_str(), pwd.length(), pwdHash))
    {
        cerr << "Error when generating SHA256 from \"" << pwd << "\"" << endl;
        return -2;
    }

    // determine the size of the world and create for every single process a status and request element
    totalProcesses=MPI::COMM_WORLD.Get_size();
    if(totalProcesses < 2)
    {
        cerr << "Insufficient number of workers: " << totalProcesses-1 << endl << "Aborting" << endl;
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    // determine which process i am
    int worldRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);


    if(worldRank == MasterProcess)
    {
        for(int i=1; i<=MaxChars; i++)
        {
            bruteRecursive(string(""), i-1, i);
        }

        cerr << "Sorry, password not found" << endl;
        // TODO: shutdown a bit more friendly
        MPI_Abort(MPI_COMM_WORLD, -2);
    }
    else
    {
        worker();
    }

    MPI_Finalize();
    return 0;
}
