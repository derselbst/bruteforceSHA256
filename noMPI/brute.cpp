#include <iostream>
#include <cstdlib>
#include <string>
#include "alphabet.h"

using namespace std;

string pwd;

static const unsigned char MaxChars = 20;

void checkPassword(string password)
{
    cout << "checking " << password << endl;

    // TODO: generate sha hash, compare hashes

    if (password==pwd)
    {
        cout << "match [" << password << "]" << endl;
        exit(0);
    }
}

void brute(string baseString, int width, int position)
{
    for(int i=0; i<SizeAlphabet; i++)
    {
        if (position+1 < width)
        {
            brute(baseString+alphabet[i], width, position+1);
        }
        
        checkPassword(baseString+alphabet[i]);
    }
}

int main()
{
    cout << "Enter a string: " << endl;
    cin >> pwd;

    for(int i=1; i<=MaxChars; i++)
    {
        cout << "checking passwords with " << i << " characters..." << endl;
        brute(string(""),i,0);
    }
    
    return 1;
}
