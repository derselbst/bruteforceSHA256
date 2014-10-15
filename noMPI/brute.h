// contains the hash of the unknown password
extern char pwdHash[SHA256_DIGEST_LENGTH];

// contains the hash of a bruteforced string
extern char bruteHash[SHA256_DIGEST_LENGTH];

// the maximum number of characters bruteforce shall check
extern const unsigned char MaxChars = 20;

int bruteInit(string password);
bool bruteIterative(const unsigned int width);
void bruteRecursive(const string baseString, const unsigned int width);
bool generateSHA256(const void *const input, const size_t &length, char *const hashStr);
