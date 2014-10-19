#include <iostream>
#include <string>
#include <openssl/sha.h>
#include <mpi.h>

using namespace std;

#include "brute.h"

extern const int MasterProcess;

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    string pwd="0<GV";

    // initialize the hash buffer for the password
    if(!generateSHA256(pwd.c_str(), pwd.length(), pwdHash))
    {
        cerr << "Error when generating SHA256 from \"" << pwd << "\"" << endl;
        return -2;
    }

    int worldRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);


    if(worldRank == MasterProcess)
    {
        // determine the size of the world and create for every single process a status and request element
        totalProcesses=MPI::COMM_WORLD.Get_size();

        if(totalProcesses < 2)
        {
            cerr << "Insufficient number of workers: " << totalProcesses-1 << endl << "Aborting" << endl;
            MPI_Abort(MPI_COMM_WORLD, -1);
        }

        bruteIterative(MaxChars);
    }
    else
    {
        worker();
    }

    MPI_Finalize();
    return 0;
}
