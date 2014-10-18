// contains the hash of the unknown password
extern char pwdHash[SHA256_DIGEST_LENGTH];

// contains the hash of a bruteforced string
extern char bruteHash[SHA256_DIGEST_LENGTH];

// the master process will be process 0
const int MaxChars = 20;

// the master process will be process 0
const int MasterProcess = 0;

bool bruteIterative(const unsigned int width);
void bruteRecursive(const string baseString, const unsigned int width);
bool generateSHA256(const void *const input, const size_t &length, char *const hashStr);
void worker();

