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
            MPI::COMM_WORLD.Abort(-3);
        }
    }
}

int main(int argc, char** argv)
{
    MPI::Init(argc, argv);

    // determine the size of the world
    totalProcesses=MPI::COMM_WORLD.Get_size();
    if(totalProcesses < 2)
    {
        cerr << "Insufficient number of workers: " << totalProcesses-1 << endl << "Aborting" << endl;
        MPI::Finalize();
        return -1;
    }

    // determine which process i am
    int worldRank = MPI::COMM_WORLD.Get_rank();

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
            MPI::COMM_WORLD.Send(&pwdHash, SHA256_DIGEST_LENGTH, MPI_BYTE, i, hash);
        }

        // start bruteforcing
        for(int i=1; i<=MaxChars; i++)
        {
            cout << "checking passwords with " << i << " characters..." << endl;
            bruteRecursive(string(""), i-1, i);
        }

        cerr << "Sorry, password not found" << endl;
        // TODO: shutdown a bit more friendly
        MPI::COMM_WORLD.Abort(-2);
    }
    else
    {
        MPI::Status state;

        // check for new msg
        MPI::COMM_WORLD.Probe(MasterProcess, MPI_ANY_TAG, state);

        MPI::COMM_WORLD.Recv(&pwdHash, SHA256_DIGEST_LENGTH, MPI_BYTE, MasterProcess, MPI_ANY_TAG, state);

        worker();
    }

    MPI::Finalize();
    return 0;
}
