#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include "disk.h"

using namespace std;

struct BPlusTreeNode {
    bool isLeafNode; // flag indicating whether/not node is a leaf node
    int numKeys; // number of keys stored in the node
    uint keys[23] = {0};
    BPlusTreeNode* children[24] = {nullptr};
    vector<vector<Record*>> recordLists; // list of records associated with each key (for leaf nodes only) (needed for duplicate handling)
    BPlusTreeNode* parent; // Pointer to parent node
};

class BPlusTree {
    public:
        BPlusTreeNode* root; // Pointer to root node
        int order; // Order of the B+ tree
        int minKeys; // Min number of keys in a node
        int maxKeys; // Max number of keys in a node

    public:
        BPlusTree(int order); // constructor
        ~BPlusTree(); // destructor

        void insert(uint key, Record* record); // Function to insert a key and its corresponding record into the B+ tree
        // void remove(int key); // Function to delete a key and its corresponding record from the B+ tree

        // Helper functions for insertion
        void insertRecursive(BPlusTreeNode* node, uint key, Record* record);
        void insertIntoLeaf(BPlusTreeNode* node, uint key, Record* record);
        void insertIntoParent(BPlusTreeNode* left, uint key, BPlusTreeNode* right);
        void splitLeafNode(BPlusTreeNode* node, uint key, Record* record);
        void splitInternalNode(BPlusTreeNode* node, uint key);
        void printEntireBPlusTree();
        tuple<vector<Record*>, int> search_target_range(uint lower_bound, uint upper_bound);
        tuple<BPlusTreeNode*, bool> search_key(uint key);
        tuple<BPlusTreeNode*,bool> searchRecursive(BPlusTreeNode* node, uint key);
        BPlusTreeNode* createLeafNode();
        BPlusTreeNode* createInternalNode();
        int countNodes(BPlusTreeNode* root);
        void printNodeAndLevelCount(BPlusTreeNode* root);
        int countLevels(BPlusTreeNode* root);
        void printBPlusTree(BPlusTreeNode* treeRoot);
        void destroyTree(BPlusTreeNode* node);
        void print_root_node();

        // Functions for deletion from B+ tree

        // 1. Main deletion function
        void remove(uint key);

        // 2. To search for the deleted key in the parent, so dont have to call a for loop everytime
        tuple<BPlusTreeNode*, bool> search_to_delete(uint key);
        tuple<int, bool> search_key_in_node(BPlusTreeNode* target_node, uint key);

        // 3. When a deleted key is found in the tree as an index, it is deleted and replaced by the smallest value of its right sub-tree
        uint find_smallest_right_subtree(BPlusTreeNode* right_ptr);

        // 5. Checks whether the leaf node is balanced, call this after the deletion has been handled
        bool leaf_is_balanced(BPlusTreeNode* leaf_node);

        // 6. Checks whether the internal node is balanced, call this after the deletion has been handled
        bool internal_is_balanced(BPlusTreeNode* internal_node);

        // 7. The borrower_node borrows a key to a borrowing_node, different behaviours for leaf nodes and internal nodes-
        bool borrowSiblings(BPlusTreeNode* borrowing_node);

        // 8. Has different behaviours for leaf nodes and internal nodes
        void mergeNodes(BPlusTreeNode* left, BPlusTreeNode* right);

        // 9. Handle the removal of the deleted key by working its way up from a node to a root
        void remove_deleted_keys(BPlusTreeNode* target_node, uint key);
};

#endif /* BPLUSTREE_H */