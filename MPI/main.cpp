/**
 * Author: derselbst
 * Description: a demo program
 */

#include <iostream>
#include <cstring> // std::memcpy()
#include <string>
#include <openssl/sha.h>
#include <mpi.h>

using namespace std;

#include "worker.h"
#include "master.h"
#include "mpiMsgTag.h"

/**
 * @brief read password or SHA hash from stdin
 *
 * if the input is SHA256_DIGEST_LENGTH bytes long, then treat it as SHA hash
 * else: cleartext password
 */
void getPassword()
{
    cout << "Enter a Password or an SHA256 Hash: ";

    string pwd;
    cin >> pwd;

    while((pwd.length() > MaxChars) && (pwd.length() != SHA256_DIGEST_LENGTH))
    {
        cerr << "Error: The Password you entered must be shorter " << (int)MaxChars << " Characters." << endl << "Try Again: ";
        cin >> pwd;
    }

    if(pwd.length() == SHA256_DIGEST_LENGTH)
    {
        memcpy(pwdHash, pwd.c_str(), SHA256_DIGEST_LENGTH);
    }
    else
    {
        // initialize the hash buffer for the password
        if(!generateSHA256(pwd.c_str(), pwd.length(), pwdHash))
        {
            cerr << "Error when generating SHA256 from \"" << pwd << "\"" << endl;
            MPI_Abort(MPI_COMM_WORLD, -3);
        }
    }
}

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    // determine the size of the world
    totalProcesses=MPI::COMM_WORLD.Get_size();
    if(totalProcesses < 2)
    {
        cerr << "Insufficient number of workers: " << totalProcesses-1 << endl << "Aborting" << endl;
        MPI_Finalize();
        return -1;
    }

    // determine which process i am
    int worldRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

    if(worldRank == MasterProcess)
    {
        /********************************************
         * This is where the MasterProcess operates *
         ********************************************/

        // read the password or hash from stdin
        getPassword();

        // send the Hash of the unknown password to all other workers
        for(int i=1; i<totalProcesses; i++)
        {
            MPI_Send(&pwdHash, SHA256_DIGEST_LENGTH, MPI_BYTE, i, hash, MPI_COMM_WORLD);
        }

        // start bruteforcing
        for(int i=1; i<=MaxChars; i++)
        {
            cout << "checking passwords with " << i << " characters..." << endl;
            bruteRecursive(string(""), i-1, i);
        }

        cerr << "Sorry, password not found" << endl;
        // TODO: shutdown a bit more friendly
        MPI_Abort(MPI_COMM_WORLD, -2);
    }
    else
    {
        MPI_Status state;

        // check for new msg
        MPI_Probe(MasterProcess, MPI_ANY_TAG, MPI_COMM_WORLD, &state);

        MPI_Recv(&pwdHash, SHA256_DIGEST_LENGTH, MPI_BYTE, MasterProcess, MPI_ANY_TAG, MPI_COMM_WORLD, &state);

        worker();
    }

    MPI_Finalize();
    return 0;
}
