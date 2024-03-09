#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include "disk.h"

using namespace std;

struct BPlusTreeNode {
    bool isLeafNode; // flag indicating whether/not node is a leaf node
    int numKeys; // number of keys stored in the node
    uint keys[23] = {0};
    BPlusTreeNode* children[24] = {nullptr};
    //vector<int> keys; // keys stored in this node
    vector<vector<Record*>> recordLists; // list of records associated with each key (for leaf nodes only) (needed for duplicate handling)
    //vector<BPlusTreeNode*> children; // Pointers to children nodes (for internal nodes only)
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

        tuple<BPlusTreeNode*,bool> search(uint key); // Function to search for a key in the B+ tree
        void insert(uint key, Record* record); // Function to insert a key and its corresponding record into the B+ tree
        void print_tree();
        // void remove(int key); // Function to delete a key and its corresponding record from the B+ tree

        // Helper functions for insertion and deletion
        // void mergeNodes(BPlusTreeNode* left, BPlusTreeNode* right);
        void insertRecursive(BPlusTreeNode* node, uint key, Record* record);
        void insertIntoLeaf(BPlusTreeNode* node, uint key, Record* record);
        void insertIntoParent(BPlusTreeNode* left, uint key, BPlusTreeNode* right);
        void splitLeafNode(BPlusTreeNode* node, uint key, Record* record);
        void splitInternalNode(BPlusTreeNode* node, uint key);
        void printEntireBPlusTree();
        tuple<BPlusTreeNode*, bool> search_key(uint key);
        tuple<BPlusTreeNode*,bool> searchRecursive(BPlusTreeNode* node, uint key);
        BPlusTreeNode* createLeafNode();
        BPlusTreeNode* createInternalNode();
        // void removeRecursive(BPlusTreeNode* node, int key);
        void printBPlusTree(BPlusTreeNode* treeRoot);
        void destroyTree(BPlusTreeNode* node);
        void print_root_node();
};

#endif /* BPLUSTREE_H */