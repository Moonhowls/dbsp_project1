#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include "disk.h"

using namespace std;

struct BPlusTreeNode {
    bool isLeafNode; // flag indicating whether/not node is a leaf node
    int numKeys; // number of keys stored in the node
    vector<int> keys; // keys stored in this node
    vector<vector<Record*>> recordLists; // list of records associated with each key (for leaf nodes only) (needed for duplicate handling)
    vector<BPlusTreeNode*> children; // Pointers to children nodes (for internal nodes only)
    BPlusTreeNode* parent; // Pointer to parent node
};

class BPlusTree {
    private:
        BPlusTreeNode* root; // Pointer to root node
        int order; // Order of the B+ tree
        int minKeys; // Min number of keys in a node
        int maxKeys; // Max number of keys in a node

    public:
        BPlusTree(int order); // constructor
        ~BPlusTree(); // destructor

        Record* search(int key); // Function to search for a key in the B+ tree
        void insert(int key, Record* record); // Function to insert a key and its corresponding record into the B+ tree
        void remove(int key); // Function to delete a key and its corresponding record from the B+ tree

        // Helper functions for insertion and deletion
        void insertIntoNode(BPlusTreeNode* node, int key, Record* record);
        void splitNode(BPlusTreeNode* node);
        BPlusTreeNode* findLeafNode(int key);
        void deleteFromNode(BPlusTreeNode* node, int key);
        void mergeNodes(BPlusTreeNode* left, BPlusTreeNode* right);
};

#endif /* BPLUSTREE_H */