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
void BPlusTree::destroyTree(BPlusTreeNode* node) {
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
Record* BPlusTree::searchRecursive(BPlusTreeNode* node, int key) {
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

BPlusTreeNode* BPlusTree::createLeafNode() {
    BPlusTreeNode* newLeafNode = new BPlusTreeNode;

    newLeafNode -> isLeafNode = true;
    newLeafNode -> numKeys = 0;
    newLeafNode -> parent = nullptr;
    return newLeafNode;
}

BPlusTreeNode* BPlusTree::createInternalNode() {
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

void BPlusTree::remove(int key) {
    // call recursive fn to delete key
    removeRecursive(root, key);
}

void BPlusTree::removeRecursive(BPlusTreeNode* node, int key) {
    if (node == nullptr) {
        return;
    }

    // find position of key in node
    int index = 0;
    while (index < node -> numKeys && key > node -> keys[index]) {
        ++index;
    }

    if (index < node -> numKeys && key == node -> keys[index]) {
        // key found in this node
        if (node -> isLeafNode) {
            // key was found in a leaf node
            // delete the key and the record from the leaf node
            node -> keys.erase(node -> keys.begin() + index);
            node -> recordLists.erase(node -> recordLists.begin() + index);
            --node -> numKeys;
        } else {
            // key was found in an internal node
            // replace the key w predecessor/successor key from a leaf node
            BPlusTreeNode* leftChildNode = node -> children[index];
            BPlusTreeNode* rightChildNode = node -> children[index + 1];

            // find predecessor key from left child node
            while (!leftChildNode -> isLeafNode) {
                leftChildNode = leftChildNode -> children.back();
            }
            int precedecessorKey = leftChildNode -> keys.back();

            // find the successor key from the right child node
            while (!rightChildNode -> isLeafNode) {
                rightChildNode = rightChildNode -> children.front();
            }
            int successorKey = rightChildNode -> keys.front();

            // choose between the predecessor and successor keys to replace the deleted key
            int replacementKey;
            if (node -> keys.size() - index <= index) {
                replacementKey = precedecessorKey;
            } else {
                replacementKey = successorKey;
            }

            // replace the key in the internal node
            node -> keys[index] = replacementKey;

            // recursively delete the key from the leaf node
            removeRecursive((replacementKey == precedecessorKey) ? leftChildNode : rightChildNode, replacementKey);
        }
    } else {
        // key not found in this node, need to continue searching in appropriate child node
        removeRecursive(node -> children[index], key);
    }
}

BPlusTreeNode* findSiblingNode(BPlusTreeNode* node) {
    if (node -> parent == nullptr) {
        return nullptr; // rootNode has no sibling
    }

    BPlusTreeNode* parentNode = node -> parent;
    
    int index = 0;
    while (index < parentNode -> children.size() && parentNode -> children[index] != node) {
        ++index;
    }

    // check if node has left sibling
    if (index > 0) {
        return parentNode -> children[index - 1];
    }

    // check if node has right sibling
    if (index < parentNode -> children.size() - 1) {
        return parentNode -> children[index + 1];
    }

    return nullptr; // the case of node having no siblings
}

void BPlusTree::mergeNodes(BPlusTreeNode* left, BPlusTreeNode* right) {
    // find the parent node
    BPlusTreeNode* parentNode = left -> parent;

    // find index of right node in the parent node
    int index = 0;
    while (index < parentNode -> children.size() && parentNode -> children[index] != right) {
        ++index;
    }

    // shift the keys and the children from the right node to the left node
    left -> keys.push_back(parentNode -> keys[index - 1]);
    left -> keys.insert(left -> keys.end(), right -> keys.begin(), right -> keys.end());
    left -> children.insert(left -> children.end(), right -> children.begin(), right -> children.end());

    // update parent nodes keys and children after the shift
    parentNode -> keys.erase(parentNode -> keys.begin() + index - 1);
    parentNode -> children.erase(parentNode -> children.begin() + index);

    // update right node children's parent pointer post shift
    for (auto child : right -> children) {
        child -> parent = left;
    }

    // delete right node
    delete right;
}

void redistributeKeys(BPlusTreeNode* left, BPlusTreeNode* right) {
    // find parent node
    BPlusTreeNode* parentNode = left -> parent;

    // find index of right node in parent node
    int index = 0;
    while (index < parentNode -> children.size() && parentNode -> children[index] != right) {
        ++index;
    }

    // redistribute keys between left and right nodes
    if (left -> keys.size() < right -> keys.size()) {
        // redistribute from right to left since right has more keys
        int redistributionIndex = right -> keys.size() / 2;
        left -> keys.push_back(parentNode -> keys[index - 1]);
        parentNode -> keys[index - 1] = right -> keys[redistributionIndex];
        left -> keys.insert(left -> keys.end(), right -> keys.begin(), right -> keys.begin() + redistributionIndex);
        right -> keys.erase(right -> keys.begin(), right -> keys.begin() + redistributionIndex);

        // check if nodes are internal nodes, because then they require children pointers to be redistributed too
        if (!left -> isLeafNode) {
            left -> children.insert(left -> children.end(), right -> children.begin(), right -> children.begin() + redistributionIndex + 1);
            right -> children.erase(right -> children.begin(), right -> children.begin() + redistributionIndex + 1);
        }
    } else {
        // redistribute keys from left to right since left has more keys
        int redistributionIndex = left -> keys.size() / 2;
        right -> keys.insert(right -> keys.begin(), parentNode -> keys[index - 1]);
        parentNode -> keys[index - 1] = left -> keys[redistributionIndex - 1];
        right -> keys.insert(right -> keys.begin(), left -> keys.begin() + redistributionIndex - 1, left -> keys.end());
        left -> keys.erase(left -> keys.begin() + redistributionIndex - 1, left -> keys.end());

        // check if nodes are internal nodes, because then they require children pointers' redistribution as well
        if (!left -> isLeafNode) {
            right -> children.insert(right -> children.begin(), left -> children.begin() + redistributionIndex, left -> children.end());
            left -> children.erase(left -> children.begin() + redistributionIndex, left -> children.end());
        }
    }
}

void BPlusTree::printEntireBPlusTree() {
    if (this == nullptr || this -> root == nullptr) {
        cout << "B+ Tree is empty" << endl;
        return;
    }

    cout << "B+ Tree: ";
    printBPlusTree(this -> root);
    cout << endl;
}

void BPlusTree::printBPlusTree(BPlusTreeNode* treeRoot) {
    if (treeRoot == nullptr) {
        return;
    }

    if (treeRoot -> isLeafNode) {
        // print keys and values of leaf node
        for (int i = 0; i < treeRoot -> numKeys; ++i) {
            cout << treeRoot -> keys[i] << ": ";
            for (int j = 0; j < treeRoot -> recordLists[i].size(); ++j) {
                Record* recordToPrint = treeRoot -> recordLists[i][j];
                cout << "(" << recordToPrint -> tconst << ", " << recordToPrint -> averageRating << ", " << recordToPrint -> numVotes << ") ";
            }
            cout << " | ";
        }
    } else {
        // recursively print child nodes
        for (int i = 0; i < treeRoot -> numKeys; ++i) {
            printBPlusTree(treeRoot -> children[i]);
            cout << treeRoot -> keys[i] << " ";
        }

        printBPlusTree(treeRoot -> children[treeRoot -> numKeys]);
    }
}