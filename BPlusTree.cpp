#include "BPlusTree.h"

/**
 * @brief Construct a new BPlusTree::BPlusTree object
 * 
 * @param order 
 */
BPlusTree::BPlusTree(int order) {
    this -> order = order;
    this -> minKeys = order / 2;
    this -> maxKeys = order - 1;
    this -> root = nullptr;
}

/**
 * @brief Destroy the BPlusTree::BPlusTree object
 * 
 */
BPlusTree::~BPlusTree() {
    destroyTree(root);
}

/**
 * @brief Helper function for the destructor
 * 
 * @param node 
 */
void destroyTree(BPlusTreeNode* node) {
    if (node == nullptr) {
        return;
    }

    if (!node -> isLeafNode) {
        for (int i = 0; i < node -> children.size(); ++i) {
            destroyTree(node -> children[i]);
        }
    }

    delete node;
}

/**
 * @brief 
 * 
 * @param key 
 * @return Record* 
 */
Record* BPlusTree::search(int key) {
    return searchRecursive(root, key);
}

/**
 * @brief 
 * 
 * @param node 
 * @param key 
 * @return Record* 
 */
Record* searchRecursive(BPlusTreeNode* node, int key) {
    if (node == nullptr) {
        return nullptr;
    }

    int index = 0;
    while (index < node -> numKeys && key == node -> keys[index]) {
        ++index;
    }

    if (index < node -> numKeys && key == node -> keys[index]) {
        // Key found in this node
        if (node -> isLeafNode) {
            // Return the first record associated with this key
            return node -> recordLists[index][0];
        } else {
            // Continue searching in the child node
            return searchRecursive(node -> children[index + 1], key);
        }
    } else {
        // Key not found in this node
        if (node -> isLeafNode) {
            return nullptr;
        } else {
            // Continue searching in the appropriate child node
            return searchRecursive(node -> children[index], key);
        }
    }
}

