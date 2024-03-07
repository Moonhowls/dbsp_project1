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

/**
 * @brief 
 * 
 * @param key 
 * @param record 
 */
void BPlusTree::insert(int key, Record* record) {
    if (root == nullptr) {
        // Tree's empty, need to create root node
        root = createLeafNode();
    }

    // Call recursive fn to handle insertion of key and record
    insertRecursive(root, key, record);
}

/**
 * @brief 
 * 
 * @param node 
 * @param key 
 * @param record 
 */
void BPlusTree::insertRecursive(BPlusTreeNode* node, int key, Record* record) {
    // check if node is a leaf node
    if (node -> isLeafNode) {
        // Call fn to insert key and record into leaf node
        insertIntoLeaf(node, key, record);
    } else {
        // Find appropriate child node to insert key and record into
        int index = 0;
        while (index < node -> numKeys && key > node -> keys[index]) {
            ++index;
        }
        // Perform recursive insertion into child node
        insertRecursive(node -> children[index], key, record);
    }
}

void BPlusTree::insertIntoLeaf(BPlusTreeNode* node, int key, Record* record) {
    // find position in which to insert key
    int index = 0;
    while (index < node -> numKeys && key > node -> keys[index]) {
        ++index;
    }

    // insert key and record in appripriate position
    node -> keys.insert(node -> keys.begin() + index, key);
    node -> recordLists.insert(node -> recordLists.begin() + index, vector<Record*>{record});
    ++node -> numKeys;

    // check if leaf node full and if so, requires splitting
    if (node -> numKeys > maxKeys) {
        splitLeafNode(node);
    }
}

void BPlusTree::splitLeafNode(BPlusTreeNode* node) {
    // Create a new leaf node
    BPlusTreeNode* newLeafNode = createLeafNode();

    // move half of keys+records into newly created leaf node
    int mid = (node -> numKeys + 1) / 2;
    newLeafNode -> keys.assign(node -> keys.begin() + mid, node -> keys.end());
    newLeafNode -> recordLists.assign(node -> recordLists.begin() + mid, node -> recordLists.end());
    newLeafNode -> numKeys = node -> numKeys - mid;
    node -> numKeys = mid;

    // update pointers accordingly
    newLeafNode -> parent = node -> parent;
    newLeafNode -> isLeafNode = true;
    newLeafNode -> children.clear();

    // connect the leaf nodes
    newLeafNode -> children.push_back(node -> children.back());
    node -> children.back() = newLeafNode;

    // update parent node if necessary
    if (node -> parent != nullptr) {
        insertIntoParent(node, newLeafNode -> keys[0], newLeafNode);
    } else {
        // create a new root node
        BPlusTreeNode* newRootNode = createInternalNode();
        newRootNode -> keys.push_back(newLeafNode -> keys[0]);
        newRootNode -> children.push_back(node);
        newRootNode -> children.push_back(newLeafNode);
        root = newRootNode;
        node -> parent = newRootNode;
        newLeafNode -> parent = newRootNode;
    }
}

BPlusTreeNode* createLeafNode() {
    BPlusTreeNode* newLeafNode = new BPlusTreeNode;

    newLeafNode -> isLeafNode = true;
    newLeafNode -> numKeys = 0;
    newLeafNode -> parent = nullptr;
    return newLeafNode;
}

BPlusTreeNode* createInternalNode() {
    BPlusTreeNode* newInternalNode = new BPlusTreeNode;

    newInternalNode -> isLeafNode = false;
    newInternalNode -> numKeys = 0;
    newInternalNode -> parent = nullptr;
    return newInternalNode;
}

void BPlusTree::insertIntoParent(BPlusTreeNode* left, int key, BPlusTreeNode* right) {
    BPlusTreeNode* parentNode = left -> parent;

    // Chcek if parent node is null (root node)
    if (parentNode == nullptr) {
        // create a new root node
        BPlusTreeNode* newRootNode = createInternalNode();
        newRootNode -> keys.push_back(key);
        newRootNode -> children.push_back(left);
        newRootNode -> children.push_back(right);
        left -> parent = newRootNode;
        right -> parent = newRootNode;
        root = newRootNode;
        return;
    }

    // find index position to insert new key
    int index = 0;
    while (index < parentNode -> numKeys && key > parentNode -> keys[index]) {
        ++index;
    }

    // insert key and pointers at appropriate positions
    parentNode -> keys.insert(parentNode -> keys.begin() + index, key);
    parentNode -> children.insert(parentNode -> children.begin() + index + 1, right);
    ++parentNode -> numKeys;

    // check if parentNode requires splitting since its full
    if (parentNode -> numKeys > maxKeys) {
        splitInternalNode(parentNode);
    }
}

void BPlusTree::splitInternalNode(BPlusTreeNode* node) {
    // create new internal node
    BPlusTreeNode* newInternalNode = createInternalNode();

    // move half of keys+pointers to newly created internal node
    int mid = (node -> numKeys + 1) / 2;
    newInternalNode -> keys.assign(node -> keys.begin() + mid + 1, node -> keys.end());
    newInternalNode -> children.assign(node -> children.begin() + mid + 1, node -> children.end());
    newInternalNode -> numKeys = node -> numKeys - mid - 1;
    node -> numKeys = mid;

    // update pointers
    newInternalNode -> parent = node -> parent;
    for (auto child : newInternalNode -> children) {
        child -> parent = newInternalNode;
    }

    // insert mid key and pointer into the aparent node
    int midKey = node -> keys[mid];
    insertIntoParent(node, midKey, newInternalNode);
}