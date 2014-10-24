#include <iostream>
#include <string>
#include <cstring>
#include <openssl/sha.h>
#include <assert.h>

using namespace std;

#include "brute.h"

extern volatile bool strFound;

int main()
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
            return -2;
        }
    }

    cout << "checking using Recusive Method" << endl;
    for(int i=1; (i<=MaxChars) && (!strFound); i++)
    {
        cout << "checking passwords with " << i << " characters..." << endl;
        bruteRecursive(string(""),i);
    }
    assert(strFound);

    cout << "checking using Iterative Method" << endl;
    assert(bruteIterative(MaxChars));



    pwd="?";
    strFound=false;

    // generate sha hash from entered string and write it to pwdHash
    if(!generateSHA256(pwd.c_str(), pwd.length(), pwdHash))
    {
        cerr << "Error when generating SHA256 from \"" << pwd << "\"" << endl;
        return -2;
    }

    cout << "checking using Recusive Method" << endl;
    for(int i=1; (i<=MaxChars) && (!strFound); i++)
    {
        cout << "checking passwords with " << i << " characters..." << endl;
        bruteRecursive(string(""),i);
    }
    assert(!strFound);

    cout << "checking using Iterative Method" << endl;
    assert(!bruteIterative(MaxChars));

    return 0;
}
