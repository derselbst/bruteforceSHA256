#include <iostream>
#include <string>
#include <openssl/sha.h>
#include <assert.h>
#include <mpi.h>

using namespace std;

#include "brute.h"

extern const int MasterProcess;

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    string pwd="000";

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
        bruteIterative(MaxChars);
    }
    else
    {
        worker();
    }

    MPI_Finalize();
    return 0;
}
