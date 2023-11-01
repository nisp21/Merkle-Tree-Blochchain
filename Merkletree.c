#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Hashing Function (Murmur3) -> Generates 32 bit unsignedT hash code
uint32_t murmur3_32(const char *key, uint32_t len, uint32_t seed)
{
    static const uint32_t c1 = 0xcc9e2d51;
    static const uint32_t c2 = 0x1b873593;
    static const uint32_t r1 = 15;
    static const uint32_t r2 = 13;
    static const uint32_t m = 5;
    static const uint32_t n = 0xe6546b64;

    uint32_t hash = seed;

    const int nblocks = len / 4;
    const uint32_t *blocks = (const uint32_t *)key;
    int i;
    for (i = 0; i < nblocks; i++)
    {
        uint32_t k = blocks[i];
        k *= c1;
        k = (k << r1) | (k >> (32 - r1));
        k *= c2;

        hash ^= k;
        hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;
    }

    const uint8_t *tail = (const uint8_t *)(key + nblocks * 4);
    uint32_t k1 = 0;

    switch (len & 3)
    {
    case 3:
        k1 ^= tail[2] << 16;
    case 2:
        k1 ^= tail[1] << 8;
    case 1:
        k1 ^= tail[0];

        k1 *= c1;
        k1 = (k1 << r1) | (k1 >> (32 - r1));
        k1 *= c2;
        hash ^= k1;
    }

    hash ^= len;
    hash ^= (hash >> 16);
    hash *= 0x85ebca6b;
    hash ^= (hash >> 13);
    hash *= 0xc2b2ae35;
    hash ^= (hash >> 16);

    return hash;
}

// Merkle Tree Node structure
struct MerkleTreeNode
{
    uint32_t HASH;
    struct MerkleTreeNode *left, *right, *prev;
};

// Function to create and initialize the tree node
struct MerkleTreeNode *createNode(char *val)
{
    struct MerkleTreeNode *newNode = (struct MerkleTreeNode *)malloc(sizeof(struct MerkleTreeNode));
    newNode->HASH = murmur3_32(val, strlen(val), 0x44EBB272);
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->prev = NULL;
    return newNode;
}

// ------------------Queue Functions---------------------
struct Qnode
{
    struct MerkleTreeNode *ptr;
    struct Qnode *next;
};
struct Qnode *front = NULL;
struct Qnode *rear = NULL;

void insertQueue(struct MerkleTreeNode *key)
{
    struct Qnode *newNode = (struct Qnode *)malloc(sizeof(struct Qnode));
    newNode->ptr = key;
    newNode->next = NULL;
    if (front == NULL)
    {
        front = newNode;
        rear = newNode;
        return;
    }
    rear->next = newNode;
    rear = rear->next;
}

struct MerkleTreeNode *deleteQueue()
{
    if (front == NULL)
        return NULL;
    struct Qnode *temp = front;
    struct MerkleTreeNode *t = front->ptr;
    front = front->next;
    free(temp);
    return t;
}

// -------------End of Queue Functions----------------

// Level-ordered traveresal of tree using queue
void level_ordered_display(struct MerkleTreeNode *root)
{
    printf("\n\n");
    insertQueue(root);
    struct MerkleTreeNode *temp = createNode("-1");
    insertQueue(temp);
    while (front != NULL)
    {
        struct MerkleTreeNode *cur = deleteQueue();
        if (cur == temp)
        {
            if (front == NULL)
            {
                printf("\n-x-x-x-x-x-x-x-\n");
                return;
            }
            insertQueue(temp);
            printf("\n\n");
            continue;
        }

        printf("%lu\t", (unsigned long)cur->HASH);
        if (cur->left != NULL)
            insertQueue(cur->left);
        if (cur->right != NULL)
            insertQueue(cur->right);
    }
}

// Block Structure
struct Block
{
    // Block Header
    uint32_t hash;
    uint32_t prevhash;
    uint32_t nonce;
    uint32_t merkle_root_hash;
    struct MerkleTreeNode *merkleRoot;
    float version;
    float timestamp;
    struct Block *next, *prev;

    // Block Content
    char transactions[4][30];
};
// Function to create and initialize the Block
struct Block *createBlock(uint32_t hash, uint32_t prevhash)
{
    struct Block *newBlock = (struct Block *)malloc(sizeof(struct Block));
    newBlock->hash = hash;
    newBlock->prevhash = prevhash;
    newBlock->next = NULL;
    newBlock->prev = NULL;

    printf("Enter the transaction data to be stored in block: \n");
    for (int i = 0; i < 4; i++)
    {
        printf("%d. ", i + 1);
        scanf("%s", &newBlock->transactions[i]);
    }

    return newBlock;
}

// Function to calculate merkle root hash
uint32_t MHash(struct Block *b)
{
    struct MerkleTreeNode *node1 = createNode(b->transactions[0]);
    struct MerkleTreeNode *node2 = createNode(b->transactions[1]);
    struct MerkleTreeNode *node3 = createNode(b->transactions[2]);
    struct MerkleTreeNode *node4 = createNode(b->transactions[3]);

    char temp1[30], temp2[30];
    sprintf(temp1, "%lu", (unsigned long)node1->HASH);
    sprintf(temp2, "%lu", (unsigned long)node2->HASH);
    struct MerkleTreeNode *node12 = createNode(strcat(temp1, temp2));
    node1->prev = node12;
    node2->prev = node12;
    node12->left = node1;
    node12->right = node2;

    sprintf(temp1, "%lu", (unsigned long)node3->HASH);
    sprintf(temp2, "%lu", (unsigned long)node4->HASH);
    struct MerkleTreeNode *node34 = createNode(strcat(temp1, temp2));
    node3->prev = node34;
    node4->prev = node34;
    node34->left = node3;
    node34->right = node4;

    sprintf(temp1, "%lu", (unsigned long)node12->HASH);
    sprintf(temp2, "%lu", (unsigned long)node34->HASH);
    b->merkleRoot = createNode(strcat(temp1, temp2));
    node12->prev = b->merkleRoot;
    node34->prev = b->merkleRoot;
    b->merkleRoot->left = node12;
    b->merkleRoot->right = node34;

    return b->merkleRoot->HASH;
}

// Function to print information of the block
void printBlock(struct Block *b)
{
    printf("Hash: %lu\n", (unsigned long)b->hash);
    printf("Previous Hash: %lu\n", (unsigned long)b->prevhash);
    b->merkle_root_hash = MHash(b);
    printf("Merkle Root Hash: %lu\n\n", (unsigned long)b->merkle_root_hash);
}

// Function to copy block
struct Block *blockCopy(struct Block *b)
{
    struct Block *newBlock = (struct Block *)malloc(sizeof(struct Block));
    newBlock->hash = b->hash;
    newBlock->prevhash = b->prevhash;
    newBlock->next = b->next;
    newBlock->prev = b->prev;

    for (int i = 0; i < 4; i++)
    {
        strcpy(newBlock->transactions[i], b->transactions[i]);
    }

    return newBlock;
}

// Driver Function
int main()
{
    // Creating a block b1
    struct Block *b1 = createBlock(548741241, 314534511);
    printBlock(b1);

    // Displaying the merkle tree of b1 level-wise
    level_ordered_display(b1->merkleRoot);

    // Creating a copy of b1
    struct Block *b1_copy = blockCopy(b1);

    // Tampering the data
    strcpy(b1_copy->transactions[1], "Transaction7");

    printBlock(b1_copy);
    level_ordered_display(b1_copy->merkleRoot);

    return 0;
}
