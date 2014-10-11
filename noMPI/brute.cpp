#include <iostream>
#include <cstdlib> // exit()
#include <cstdio> // printf()
#include <cstring> // strcmp()
#include <string>
#include <stack>
#include <openssl/sha.h>
#include <byteswap.h>
#include "alphabet.h"

using namespace std;

string pwd;

char pwdHash[SHA256_DIGEST_LENGTH];
char bruteHash[SHA256_DIGEST_LENGTH];

static const unsigned char MaxChars = 3;

void printSHAHash(unsigned int* pbuf)
{
    // byteswap the integer pointed to, to display hex dump in correct order
    // TODO: how to deal with big endian machines
    printf("%X%X%X%X%X%X%X%X\n",
           bswap_32(*(pbuf)),
           bswap_32(*(pbuf+1)),
           bswap_32(*(pbuf+2)),
           bswap_32(*(pbuf+3)),
           bswap_32(*(pbuf+4)),
           bswap_32(*(pbuf+5)),
           bswap_32(*(pbuf+6)),
           bswap_32(*(pbuf+7))
          );
}

bool generateSHA256(const void *const inputStr, const size_t &length, char *const hashStr)
{
    SHA256_CTX hash;
    if(!SHA256_Init(&hash))
    {
        return false;
    }

    if(!SHA256_Update(&hash, inputStr, length))
    {
        return false;
    }

    if(!SHA256_Final(reinterpret_cast<unsigned char*>(hashStr), &hash))
    {
        return false;
    }

    return true;
}

void checkPassword(const string &password)
{
//#ifdef VERBOSE
    cout << "checking " << password << endl;
//#endif // VERBOSE

    // generate sha hash from entered string and write it to pwdHash
    if(!generateSHA256(password.c_str(), password.length(), bruteHash))
    {
        cerr << "Error when generating SHA256 from \"" << password << "\"" << endl;
        //return;
    }

    if (!memcmp(bruteHash, pwdHash, SHA256_DIGEST_LENGTH))
    {
        cout << "match [" << password << "]" << endl << "hash: " << endl;
        printSHAHash((unsigned int*)bruteHash);
        exit(0);
    }
}

void bruteRecursive(const string baseString, const unsigned int width)
{
    for(int i=0; i<SizeAlphabet; i++)
    {
        if (baseString.length()+1 < width)
        {
            bruteRecursive(baseString+alphabet[i], width);
        }

        checkPassword(baseString+alphabet[i]);
    }
}

void bruteIterative(const unsigned int width)
{
    stack<string> myStack;

    // myStack must contain at least one element when entering loop
    // else: SIGSEGV
    // hence, start checking with an empty string
    myStack.push("");

    do
    {
        string baseString = myStack.top();
        myStack.pop();
        cout << "checking passwords with " << baseString.length()+1 << " characters..." << endl;
        for(int i=0; i<SizeAlphabet; i++)
        {
            if (baseString.length()+1 < width)
            {
                myStack.push(baseString+alphabet[i]);
            }

            checkPassword(baseString+alphabet[i]);
        }
    }
    while(!myStack.empty());
}

int main()
{
    cout << "Enter a string: " << endl;
    cin >> pwd;

    // generate sha hash from entered string and write it to pwdHash
    if(!generateSHA256(pwd.c_str(), pwd.length(), pwdHash))
    {
        cerr << "Error when generating SHA256 from \"" << pwd << "\"" << endl;
        return -2;
    }
    else
    {
        printf("SHA256 Hash for your string is:\n");
        printSHAHash((unsigned int*)pwdHash);

    }

    cout << "checking using Recusive Method" << endl;
    for(int i=1; i<=MaxChars; i++)
    {
        cout << "checking passwords with " << i << " characters..." << endl;
        bruteRecursive(string(""),i);
    }

    cout << "checking using Iterative Method" << endl;
    bruteIterative(MaxChars);

    return -1;
}
