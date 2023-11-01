# Merkle-Tree-Blochchain
Implementing Merkle Tree Used in Blockchain for Security and Error Detection


Introduction to merkle tree:

The Merkle tree, also referred to as a hash tree, was invented by Ralph Merkle in the 1980s. Merkle tree is a particular kind of structure that can be used to quickly check the accuracy of data in a set. The majority of the components in this structure are hash functions, which are widely employed in blockchain technology. Because peer-to-peer (P2P) networks require information to be exchanged and independently evaluated, Merkle trees are frequently employed in P2P networks. When considering the Bitcoin network in particular, these trees are effectively employed to validate data. This is made possible by using hashes to present data rather than an entire information file. Using a Merkle tree to validate transactions has numerous advantages for blockchain technology and cryptocurrency platforms, from effective verification to simple tamper detection.

About the code:

● Merkle tree is widely used in Blockchain Technology for data validation and also in Version Control Systems like Git.

● In blockchain technology, a copy of blockchain is passed to every individual. So if any malicious person tries to tamper with data in any block and tries to justify it, the merkle tree hash of the original block differs from the tempered block and we can easily verify if the data has been tampered or not.

● So because of the merkle tree, it is much faster to verify data integrity rather than checking all the transactions individually.

● It is also faster to locate which transaction data has been changed with the complexity of O(log n) with the help of a merkle tree.

In the driver code above, we have created a block and its copy. Now if we try to tamper the data in a block, as we change the data, its hash is changed immediately and it is reflected in a merkle root.
