#include <iostream>
#include <cstring> // memcmp()
#include <string>
#include <queue>
#include <iomanip>
#include <openssl/sha.h>
#include <byteswap.h>

using namespace std;

#include "brute.h"
#include "alphabet.h"

// clear text password entered by user
string pwd;

// contains the hash of the unknown password
char pwdHash[SHA256_DIGEST_LENGTH];

// contains the hash of a bruteforced string
char bruteHash[SHA256_DIGEST_LENGTH];

/**
 * @brief prints 32 bytes of memory
 *
 * prints a hex dump of 32 bytes of memory pointed to
 *
 * @param[in]   pbuf: pointer to some memory, usually containing an SHA256 hash
 */
void printSHAHash(const unsigned int *const pbuf)
{
    // byteswap the integer pointed to, to display hex dump in correct order
    // TODO: how to deal with big endian machines
    cout << std::hex << std::uppercase << setw(8) << setfill('0')
        << bswap_32(*(pbuf))
        << bswap_32(*(pbuf+1))
        << bswap_32(*(pbuf+2))
        << bswap_32(*(pbuf+3))
        << bswap_32(*(pbuf+4))
        << bswap_32(*(pbuf+5))
        << bswap_32(*(pbuf+6))
        << bswap_32(*(pbuf+7))
        << endl;
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

    if(!SHA256_Final(reinterpret_cast<unsigned char*>(hashStr), &hash))
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
    if(!generateSHA256(password.c_str(), password.length(), bruteHash))
    {
        cerr << "Error when generating SHA256 from \"" << password << "\"" << endl;
        return false;
    }

    if (!memcmp(bruteHash, pwdHash, SHA256_DIGEST_LENGTH))
    {
        cout << "match [" << password << "]" << endl << "hash: " << endl;
        printSHAHash((unsigned int*)bruteHash);
        return true;
    }

    return false;
}

/**
 * @brief recursive implementation of bruteforce
 *
 * recursive implementation of bruteforce attack
 * call it as follows: bruteRecursive(string(""), width);
 *
 * @param[in]   baseString: a const string indicates the prefix of a string to be checked
 * @param[in]   width:      the maximum number of characters you wish to be checked
 */
volatile bool strFound = false;
void bruteRecursive(const string baseString, const unsigned int width)
{
    for(int i=0; (i<SizeAlphabet) && (!strFound); i++)
    {
        if (baseString.length()+1 < width)
        {
            bruteRecursive(baseString+alphabet[i], width);
        }

        if(checkPassword(baseString+alphabet[i]))
        {
            strFound = true;
        }
    }
}

/**
 * @brief iterative implementation of bruteforce
 *
 * generates all baseString with 1,2,3,...,width-1 characters
 * call it as follows: bruteIterative(width);
 *
 * @param[in]   width:      the maximum number of characters you wish to be checked
 *
 * @return return true if the password was found
 * 
 * @warning PROGRAM MAY RUN OUT OF MEMORY
 * this is a rather bad implementation, since saving all variations of a strings
 * needs huge amounts of memory to hold them
 */
bool bruteIterative(const unsigned int width)
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

            if(checkPassword(baseString+alphabet[i]))
            {
                return true;
            }
        }
    }
    while(!myQueue.empty());
    return false;
}

int bruteInit(string password)
{
    pwd=password;

    // generate sha hash from entered string and write it to pwdHash
    if(!generateSHA256(pwd.c_str(), pwd.length(), pwdHash))
    {
        cerr << "Error when generating SHA256 from \"" << pwd << "\"" << endl;
        return -2;
    }
    else
    {
        cout << "SHA256 Hash for secret password is:" << endl;
        printSHAHash((unsigned int*)pwdHash);

    }

    cout << "checking using Recusive Method" << endl;
    for(int i=1; (i<=MaxChars) && (!strFound); i++)
    {
        cout << "checking passwords with " << i << " characters..." << endl;
        bruteRecursive(string(""),i);
    }

    cout << "checking using Iterative Method" << endl;
    bruteIterative(MaxChars);

    return 0;
}
