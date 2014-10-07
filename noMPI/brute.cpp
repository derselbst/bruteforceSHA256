#include <iostream>
#include <cstdlib>
#include <string>
#include "alphabet.h"

using namespace std;

wstring pwd;

static const unsigned char MaxChars = 20;

void checkPassword(wstring password)
{
    wcout << "checking " << password << endl;

    // TODO: generate sha hash, compare hashes

    if (password==pwd)
    {
        wcout << "match [" << password << "]" << endl;
        exit(0);
    }
}

void brute(wstring baseString, int width, int position)
{
    for(int i=0; i<SizeAlphabet; i++)
    {
        if (position < width-1)
        {
            brute(baseString+alphabet[i], width, position+1);
        }
        checkPassword(baseString+alphabet[i]);
    }
}

int main()
{
    wcout << "Enter a string: " << endl;
    wcin >> pwd;

    for(int i=1; i<=MaxChars; i++)
    {
        wcout << "checking passwords with " << i << " characters..." << endl;
        brute(wstring(L""),i,0);
    }
    return 1;
}
