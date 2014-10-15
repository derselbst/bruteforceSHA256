#include <iostream>
#include <string>
#include <openssl/sha.h>
#include <assert.h>

using namespace std;

#include "brute.h"

extern volatile bool strFound;

int main()
{

    string pwd="<,6F";

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
    assert(strFound);

    cout << "checking using Iterative Method" << endl;
    assert(bruteIterative(MaxChars));

    
    
    pwd="?";

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
