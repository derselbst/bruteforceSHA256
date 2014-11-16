/**
 * Author: derselbst
 * Description: a demo program
 */

#include <iostream>
#include <cstring> // std::memcpy()
#include <string>
#include <exception>
#include <openssl/sha.h>
#include <mpi.h>
#include <byteswap.h>
#include <cstdint>

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
    try
    {
        cout << "Enter a Password or an SHA256 Hash: ";

        string pwd;
        cin >> pwd;
        char tries = 2;
        while((pwd.length() > MaxChars) && (pwd.length() != SHA256_DIGEST_LENGTH*2))
        {
            cerr << "Error: The Password you entered must be shorter " << (int)MaxChars << " Characters." << endl << "Try Again: ";
            cin >> pwd;

            // avoid spamming
            if(tries > 5)
            {
                    cerr << "Error: Too many wrong inputs." << endl;
                    MPI::COMM_WORLD.Abort(-5);
            }
            tries++;
        }

        // the provided password seems to be an SHA hash
        if(pwd.length() == SHA256_DIGEST_LENGTH*2)
        {
            for(int i=0; i < SHA256_DIGEST_LENGTH/sizeof(uint32_t); i++)
            {
                short start = i*sizeof(uint32_t)*2;
                pwdHash.mem[i] = bswap_32(stol(pwd.substr(start, 8), nullptr, 16));
            }
            cout << "The SHA256 Hash of your unknown password is: ";
            printSHAHash(pwdHash.mem);
        }
        else
        {
            // initialize the hash buffer for the password
            if(!generateSHA256(pwd.c_str(), pwd.length(), pwdHash.c))
            {
                cerr << "Error when generating SHA256 from \"" << pwd << "\"" << endl;
                MPI::COMM_WORLD.Abort(-3);
            }
        }
    }
    catch(exception e)
    {
        cerr << "Error: " << e.what() << endl;
        MPI::COMM_WORLD.Abort(-4);
    }
}

void usage(char *name)
{
    // printing out usage to stdout
    cout << "usage of " << name << endl;
    cout << " Please start it with: mpirun -np N " << name << " , where N is" << endl;
    cout << " the total number of processes." << endl;
    cout << " This program bruteforces an entered clear-text-password (by checking all possible SHA256 hashes)" << endl;
    cout << " or a given SHA256-Hash (in HEX representation, exactly 64 Bytes long)." << endl << endl;

    cout << " HINT: It is not recommend to start this program without 'mpirun'" << endl;
    cout << "       or with any paramters. If you start it with ones, it will" << endl;
    cout << "       cause an error and print out this message." << endl;
}


int main(int argc, char** argv)
{
    MPI::Init(argc, argv);

    // cancel if this programm has any paramters
    if(argc > 1)
    {
    	// only one process prints out the usage 
    	if(MPI::COMM_WORLD.Get_rank() == MasterProcess)
	{
	    usage(argv[0]);
	}
    	MPI::Finalize();
    	return -1;
    }
    else
    {
	    // determine the size of the world
	    totalProcesses=MPI::COMM_WORLD.Get_size();
	    
	    // determine which process i am
	    int worldRank = MPI::COMM_WORLD.Get_rank();
	    
	    if(totalProcesses < 2)
	    {
		cerr << "Insufficient number of workers: " << totalProcesses-1 << endl << "Aborting" << endl;

		// only the masterProcess should print out the usage
		if(worldRank == MasterProcess)
		{
		    usage(argv[0]);
		}
		MPI::Finalize();
		return -1;
	    }

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
		    MPI::COMM_WORLD.Send(&pwdHash, SHA256_DIGEST_LENGTH, MPI_BYTE, i, MpiMsgTag::hash);
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
		/**************************************************
		 * This is where all the Worker Processes operate *
		 **************************************************/
		MPI::Status state;

		// check for new msg
		MPI::COMM_WORLD.Probe(MasterProcess, MPI_ANY_TAG, state);

		MPI::COMM_WORLD.Recv(&pwdHash, SHA256_DIGEST_LENGTH, MPI_BYTE, MasterProcess, MPI_ANY_TAG, state);

		worker();
	    }

	    MPI::Finalize();
	    return 0;
    }
}
