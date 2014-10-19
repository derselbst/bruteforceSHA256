// the master process will be process 0
const int MaxChars = 20;

// the master process will be process 0
const int MasterProcess = 0;

extern int totalProcesses;

bool bruteIterative(const unsigned int width);
void CallMPIProcess(string guessedPwd);
