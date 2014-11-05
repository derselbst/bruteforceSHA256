// contains the hash of the unknown password
extern char pwdHash[SHA256_DIGEST_LENGTH];

// contains the hash of a bruteforced string
extern char bruteHash[SHA256_DIGEST_LENGTH];

bool generateSHA256(const void *const input, const size_t &length, char *const hashStr);
void worker();

