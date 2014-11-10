typedef union _SHA256Hash
{
        char c[SHA256_DIGEST_LENGTH];
        uint32_t mem[SHA256_DIGEST_LENGTH/sizeof(uint32_t)];
} SHA256Hash;

// contains the hash of the unknown password
extern SHA256Hash pwdHash;

// contains the hash of a bruteforced string
extern SHA256Hash bruteHash;

void printSHAHash(const uint32_t *const pbuf);
bool generateSHA256(const void *const input, const size_t &length, char *const hashStr);
void worker();
