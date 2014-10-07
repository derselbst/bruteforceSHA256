#ifndef ALLOWEDCHARS_H_INCLUDED
#define ALLOWEDCHARS_H_INCLUDED

const int SizeAllowedChars = 10 + 26 + 26 + 16;

const wchar_t allowedChars[SizeAllowedChars]=
{
    '0',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    'a',
    'b',
    'c',
    'd',
    'e',
    'f',
    'g',
    'h',
    'i',
    'j',
    'k',
    'l',
    'm',
    'n',
    'o',
    'p',
    'q',
    'r',
    's',
    't',
    'u',
    'v',
    'w',
    'x',
    'y',
    'z',
    'A',
    'B',
    'C',
    'D',
    'E',
    'F',
    'G',
    'H',
    'I',
    'J',
    'K',
    'L',
    'M',
    'N',
    'O',
    'P',
    'Q',
    'R',
    'S',
    'T',
    'U',
    'V',
    'W',
    'X',
    'Y',
    'Z',
    '-',
    '+',
    ',',
    ';',
    '.',
    ':',
    '_',
    '!',
    L'§', // TODO: brauchen wir wirklich nur wegen diesem Mist nen wchar Array
    '$',
    '%',
    '&',
    '=',
    '*',
    '<',
    '>'
};

#endif // ALLOWEDCHARS_H_INCLUDED