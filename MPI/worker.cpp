/**
 * Author: derselbst
 * Description: provides functions that are used by the worker processes to check whether a bruteforced string generated by the master process matches pwdHash
 */

#include <iostream>
#include <cstring> // std::memcmp()
#include <string>
#include <iomanip>
#include <openssl/sha.h>
#include <byteswap.h>
#include <mpi.h>

using namespace std;

#include "worker.h"
#include "master.h"
#include "alphabet.h"

// contains the hash of the unknown password
SHA256Hash pwdHash;

// contains the hash of a bruteforced string
SHA256Hash bruteHash;

/**
 * @brief prints 32 bytes of memory
 *
 * prints a hex dump of 32 bytes of memory pointed to
 *
 * @param[in]   pbuf: pointer to some memory, usually containing an SHA256 hash
 */
void printSHAHash(const uint32_t *const pbuf)
{
    // byteswap the integer pointed to, to display hex dump in correct order
    // TODO: how to deal with big endian machines
    cout << hex << uppercase << setw(8) << setfill('0');

    for(char i=0; i < SHA256_DIGEST_LENGTH/sizeof(uint32_t); i++)
    {
        cout << bswap_32(pbuf[i]);
    }
    cout << endl;
}

/**
 * @brief generates an SHA256 hash
 *
 * generates an SHA256 hash using openSSL
 *
 * @param[in]      input:   a const pointer to const block of data, usually a char array of which the hash is being generated
 * @param[in]      length:  the number of bytes the that input points to holds
 * @param[in,out]  hashStr: const pointer to an array of SHA256_DIGEST_LENGTH bytes that will receive the hash
 *
 * @return returns true if the hash has been generated successfully; returns false if input or hashStr is NULL or length==0; else: false
 */
bool generateSHA256(const void *const input, const size_t &length, char *const hashStr)
{
    if(!hashStr || !input || length==0)
    {
        return false;
    }

    SHA256_CTX hash;
    if(!SHA256_Init(&hash))
    {
        return false;
    }

    if(!SHA256_Update(&hash, input, length))
    {
        return false;
    }

    if(!SHA256_Final(reinterpret_cast<unsigned char *const>(hashStr), &hash))
    {
        return false;
    }

    return true;
}

/**
 * @brief checks equality of two hashes
 *
 * calculates the SHA256 hash of 'password' and compares it
 * with the initial password hash
 *
 * @param[in]   password: a const string containing a guessed password
 *
 * @return returns true if hashes match; false if generation of hash failed or hashes not match
 */
bool checkPassword(const string &password)
{
#ifdef VERBOSE
    cout << "checking " << password << endl;
#endif // VERBOSE

    // generate sha hash from entered string and write it to pwdHash
    if(!generateSHA256(password.c_str(), password.length(), bruteHash.c))
    {
        cerr << "Error when generating SHA256 from \"" << password << "\"" << endl;
        return false;
    }

    if (!memcmp(bruteHash.c, pwdHash.c, SHA256_DIGEST_LENGTH))
    {
        cout << "match [" << password << "]" << endl << "hash: " << endl;
        printSHAHash(bruteHash.mem);
        return true;
    }

    return false;
}

/**
 * @brief the main loop for a worker process
 *
 * continuously looks for incoming strings, generates the SHA
 * hash of it and checks if it matches the secret hash
 */
void worker()
{
    char buf[MaxChars];
    MPI::Status state;

    while(true)
    {
        // check for new msg
        MPI::COMM_WORLD.Probe(MasterProcess, MPI_ANY_TAG, state);

        // now check status to determine how many bytes were actually received
        int len = state.Get_count(MPI_BYTE);

        // receive len bytes
        MPI::COMM_WORLD.Recv(buf, len, MPI_BYTE, MasterProcess, MPI_ANY_TAG, state);

        string baseStr(buf, len);

        for(int i=0; i<SizeAlphabet; i++)
        {
            if(checkPassword(baseStr+alphabet[i]))
            {
                cout << "Password found: " << baseStr+alphabet[i] << endl;
                MPI::COMM_WORLD.Abort(0);

                break;
            }
        }
    }
}
