// specifies the type of messages sent to workers
enum MpiMsgTag
{
    task, // indicates that a baseString was sent to a worker
    hash, // indicates that the secret hash was sent to a worker
    success // worker says: hashes match; unused ATM
};
