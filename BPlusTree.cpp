#include "BPlusTree.h"
#include <queue>

/**
 * @brief Construct a new BPlusTree::BPlusTree object
 * 
 * @param order 
 */
std::vector<uint> arrayToVector(uint arr[], int size) {
    std::vector<uint> vec(arr, arr + size);
    return vec;
}

// Function to convert vector to array
void vectorToArray(const std::vector<uint>& vec, uint arr[], int size) {
    if (size > vec.size()) {
        // Pad with zeros
        for (size_t i = 0; i < vec.size(); i++) {
            arr[i] = vec[i];
        }
        for (size_t j = vec.size(); j < size; j++) {
            arr[j] = 0;
        }
        return;
    }
    for (size_t i = 0; i < vec.size(); ++i) {
        arr[i] = vec[i];
    }
}

std::vector<BPlusTreeNode*> ptrarrayToVector(BPlusTreeNode* arr[], int size) {
    std::vector<BPlusTreeNode*> vec(arr, arr + size);
    return vec;
}

// Function to convert vector to array
void ptrvectorToArray(const std::vector<BPlusTreeNode*>& vec, BPlusTreeNode* arr[], int size) {
    if (size > vec.size()) {
        for (size_t i = 0; i < vec.size(); i++) {
            arr[i] = vec[i];
        }
        for (size_t j = vec.size(); j < size; j++) {
            arr[j] = nullptr;
        }
    }
    for (size_t i = 0; i < vec.size(); i++) {
        arr[i] = vec[i];
    }
}


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
        for (int i = 0; i < node -> numKeys + 1; ++i) {
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
// Record* BPlusTree::search(int key) {
//     return searchRecursive(root, key);
// }

// /**
//  * @brief 
//  * 
//  * @param node 
//  * @param key 
//  * @return Record* 
//  */
// Record* BPlusTree::searchRecursive(BPlusTreeNode* node, int key) {
//     if (node == nullptr) {
//         return nullptr;
//     }

//     int index = 0;
//     while (index < node -> numKeys && key == node -> keys[index]) {
//         ++index;
//     }

//     if (index < node -> numKeys && key == node -> keys[index]) {
//         // Key found in this node
//         if (node -> isLeafNode) {
//             // Return the first record associated with this key
//             return node -> recordLists[index][0];
//         } else {
//             // Continue searching in the child node
//             return searchRecursive(node -> children[index + 1], key);
//         }
//     } else {
//         // Key not found in this node
//         if (node -> isLeafNode) {
//             return nullptr;
//         } else {
//             // Continue searching in the appropriate child node
//             return searchRecursive(node -> children[index], key);
//         }
//     }
// }

// Searches the BPlusTree and returns:
// BPlusTreeNode* : a node pointer if a target is found
// bool: whether the key is a duplicate
tuple<BPlusTreeNode*, bool> BPlusTree::search(uint key) {
    return searchRecursive(root, key);
}

/**
 * @brief 
 * 
 * @param node 
 * @param key 
 * @return Record* 
 */
tuple<BPlusTreeNode*, bool> BPlusTree::search_key(uint key) {

    if (root == nullptr) {
        // Tree's empty, need to create root node
        root = createLeafNode();
        return make_tuple(root, false);
    }

    BPlusTreeNode* node = root;

    while (!(node -> isLeafNode)){
        int index = 0;
        for (int i = 0; i < node -> numKeys; i++){
            if (key < node -> keys[i]){
                // follow this pointer
                index = i;
                break;
            }
            if (i == node -> numKeys - 1){
                index = node -> numKeys;
            }
        }
        node = node->children[index];
    }

    for (int i = 0; i < node -> numKeys; i++){
        if (key == node -> keys[i]){
            return make_tuple(node, true);
        }
    }
    return make_tuple(node, false);
}

// Given a range of keys, find and return a vector of Record pointers and the number of index nodes accessed 
tuple<vector<Record*>, int> BPlusTree::search_target_range(uint lower_bound, uint upper_bound) {

    // Follow lower_bound until leaf node
    vector<Record*> record_vector;
    int index_nodes_accessed = 0;

    if (root == nullptr) {
        return make_tuple(record_vector, index_nodes_accessed);
    }

    BPlusTreeNode* node = root;

    while (!(node -> isLeafNode)){
        index_nodes_accessed++;
        int index = 0;
        for (int i = 0; i < node -> numKeys; i++){
            if (lower_bound < node -> keys[i]){
                // follow this pointer
                index = i;
                break;
            }
            if (i == node -> numKeys - 1){
                index = node -> numKeys;
            }
        }
        node = node->children[index];
    }

    BPlusTreeNode* leafNode = node; // To traverse the appropriate leaf nodes

    int ptr_array_size = sizeof(leafNode -> children)/sizeof(leafNode -> children[0]);

    while (leafNode != nullptr && upper_bound >= leafNode -> keys[0]) {
        index_nodes_accessed++;
        // Traverse leaf node
        for (int i = 0; i < leafNode->numKeys; ++i) {
            int key = leafNode->keys[i];
            if (key >= lower_bound && key <= upper_bound) {
                // Key falls within the range, print or store it
                for (int j = 0; j < leafNode -> recordLists[i].size() ; j++){
                    record_vector.push_back(leafNode -> recordLists[i][j]);
                }
            }
        }
        // Move to the next leaf node
        leafNode = leafNode->children[ptr_array_size - 1];
    }

    return make_tuple(record_vector, index_nodes_accessed);

    // if (leaf_node -> children[ptr_array_size - 1] != nullptr){
    //     for (int i = 0; i < leaf_node -> numKeys; i++){
    //         if (lower_bound >= leaf_node -> keys[i] && leaf_node -> keys[i] < upper_bound){
    //             for (int j = 0; j < leaf_node -> recordLists[i].size() ; j++){
    //                 record_vector.push_back(leaf_node -> recordLists[i][j]);
    //             }
    //         }
    //     }
    //     leaf_node = leaf_node -> children[ptr_array_size - 1];
    // }
}

tuple<BPlusTreeNode*, bool> BPlusTree::searchRecursive(BPlusTreeNode* node, uint key) {

    if (root == nullptr) {
        // Tree's empty, need to create root node
        root = createLeafNode();
        return make_tuple(root, false);
    }

    if (node -> isLeafNode){ //dont go any further down, search the leaf node
        for (int i = 0; i < node->numKeys; i++){
            if (key == node->keys[i]){
                return make_tuple(node, true);
            }
        }
        return make_tuple(node, false); // key is not in B+ tree
    }
    else{ // if not leaf node, follow the correct pointer
        int index = 0;
        for (int i = 0; i < node -> numKeys; i++){
            if (key < node -> keys[i]){
                // follow this pointer
                index = i;
                break;
            }
            if (i == node -> numKeys - 1){
                index = node -> numKeys;
            }
        }
        node = node->children[index];
        searchRecursive(node, key);
    }
} 

    // if (index < node -> numKeys && key == node -> keys[index]) {
    //     // Key found in this node
    //     if (node -> isLeafNode) {
    //         // Return the first record associated with this key
    //         return node;
    //     } else {
    //         // Continue searching in the child node
    //         return searchRecursive(node, key);
    //     }
    // } else {
    //     // Key not found in this node
    //     if (node -> isLeafNode) {
    //         return nullptr;
    //     } else {
    //         // Continue searching in the appropriate child node
    //         return searchRecursive(node, key);
    //     }
    // }
// }

/**
 * @brief 
 * 
 * @param key 
 * @param record 
 */
void BPlusTree::insert(uint key, Record* record) {

    // Search for the key, if exists, that means a vector of record pointers already exists
    BPlusTreeNode* node_ptr;
    bool is_duplicate;
    tie(node_ptr, is_duplicate) = search_key(key);

    if (node_ptr -> numKeys == 0){
        // Insert and return
        node_ptr -> keys[0] = key;
        node_ptr -> recordLists.push_back(vector<Record*>{record});
        node_ptr -> numKeys = 1;
        return;
    }

    if (is_duplicate){ // Just push_back the recordsList[i] and return
        for (int i = 0; i < node_ptr -> numKeys; i++){
            if (key == node_ptr->keys[i]){
                node_ptr -> recordLists[i].push_back(record);
                return;
            }
        }
    }

    // Call recursive fn to handle insertion of key and record
    insertRecursive(node_ptr, key, record);
}

/**
 * @brief 
 * 
 * @param node 
 * @param key 
 * @param record 
 */
void BPlusTree::insertRecursive(BPlusTreeNode* node, uint key, Record* record) {
    // check if node is a leaf node
    if (node -> isLeafNode) {
        // Call fn to insert key and record into leaf node
        insertIntoLeaf(node, key, record);
        return;
    } else {
        // Find appropriate child node to insert key and record into
        int index = 0;
        for (int i = 0; i < node -> numKeys; i++){
            if (key < node -> keys[i]){
                // follow this pointer
                index = i;
                break;
            }
            if (i == node -> numKeys - 1){
                index = node -> numKeys;
            }
        }
        // Perform recursive insertion into child node
        insertRecursive(node -> children[index], key, record);
    }
}

void BPlusTree::insertIntoLeaf(BPlusTreeNode* node, uint key, Record* record) {

    int key_array_size = sizeof(node->keys)/sizeof(node->keys[0]);
    int ptr_array_size = sizeof(node->children)/sizeof(node->children[0]);

    // check if leaf node full and if so, requires splitting
    // SplitLeafNode handles the splitting, as well as the insertion to parent
    // Once it comes back, just call return
    if (node -> numKeys == maxKeys) {
        splitLeafNode(node, key, record);
        return;
    }

    // If not full
    // find position in which to insert key 
    int index = 0;
    for (int i = 0; i < node->numKeys; i++){
        if (key < node->keys[i]){
            index = i;
            break;
        }
        if (i == node->numKeys - 1){
            index = node->numKeys;
        }
    }

    // Convert keys array to vector
    vector<uint> key_vector = arrayToVector(node->keys, node->numKeys);

    // insert key and record in appripriate position 
    key_vector.insert(key_vector.begin() + index, key);
    vectorToArray(key_vector, node->keys, key_array_size);

    // node -> keys.insert(node -> keys.begin() + index, key);
    // Inserting into a non-full leaf
    node -> recordLists.insert(node -> recordLists.begin() + index, vector<Record*>{record});

    // It's wrong here, the order is off for record list
    // cout << "The record list of first node: " << endl;
    // for (int i = 0; i < node -> recordLists.size(); i++){
    //     for (int j = 0; j < node-> recordLists[i].size(); j++){
    //         Record* record_ptr = node -> recordLists[i][j];
    //         cout << "Record " << record_ptr->tconst << ", numVotes: " << record_ptr->numVotes << endl;
    //     }
    // }

    ++node -> numKeys;

    // Deallocate memory for keys
    vector<uint>().swap(key_vector);

    // for (int i = 0; i < node -> numKeys; i++){
    //         cout << node->keys[i] << endl;
    // };
}

void BPlusTree::splitLeafNode(BPlusTreeNode* node, uint key, Record* record) {
    // Create a new leaf node
    BPlusTreeNode* newLeafNode = createLeafNode();

    // Convert keys array to vector
    vector<uint> node_vector = arrayToVector(node->keys, node->numKeys);
    vector<uint> new_node_vector;

    // move half of keys+records into newly created leaf node
    int mid = ceil(node -> numKeys + 1) / 2;
    int key_array_size = sizeof(node->keys)/sizeof(node->keys[0]);
    int ptr_array_size = sizeof(node->children)/sizeof(node->children[0]);
    int array_size = sizeof(node -> keys)/sizeof(node->keys[0]);


    // 1. Find location to insert key into node_vector
    int index;
    for (int i = 0; i < node->numKeys; i++){
        if (key < node_vector[i]){
            index = i;
            break;
        }
        if (i == node->numKeys - 1){
            index = node->numKeys;
        }
    }

    // 2. Split and assign keys to both original node and new vector
    node_vector.insert(node_vector.begin() + index, key);
    new_node_vector.assign(node_vector.begin() + mid, node_vector.end());
    vectorToArray(new_node_vector, newLeafNode -> keys, key_array_size);
    node_vector.resize(mid);
    vectorToArray(node_vector, node -> keys, key_array_size);


    // Assign keys to the new node
    // new_node_vector.assign(node_vector.begin() + mid, node_vector.end());
    // vectorToArray(new_node_vector, newLeafNode -> keys, key_array_size);
    // node_vector.resize(mid);
    // vectorToArray(node_vector, node->keys, key_array_size);
    // newLeafNode -> keys.assign(node -> keys.begin() + mid, node -> keys.end());
    node -> recordLists.insert(node -> recordLists.begin() + index, vector<Record*>{record});
    newLeafNode -> recordLists.assign(node -> recordLists.begin() + mid, node -> recordLists.end());
    node -> recordLists.resize(mid);

    // cout << "The record list of second node: " << endl;
    // for (int i = 0; i < newLeafNode -> recordLists.size(); i++){
    //     for (int j = 0; j < newLeafNode -> recordLists[i].size(); j++){
    //         Record* record_ptr = newLeafNode -> recordLists[i][j];
    //         cout << "Record " << record_ptr->tconst << ", numVotes: " << record_ptr->numVotes << endl;
    //     }
    // }

    newLeafNode -> numKeys = node -> numKeys - mid;
    node -> numKeys = mid;

    // update pointers accordingly
    newLeafNode -> parent = node -> parent;
    newLeafNode -> isLeafNode = true;

    // Update newLeafNode with the ptr to the next leaf
    if (node->children[ptr_array_size - 1] != nullptr){
        newLeafNode -> children[ptr_array_size - 1] = node -> children[ptr_array_size - 1];
    }
    // if (node->children.size() != 0){ //The target node was pointing to another node, need to update newLeafNode with that Node pointer
    //     newLeafNode -> children.push_back(node -> children.back());
    // }

    // Its trying to get children, but leaf node has no children
    // connect the leaf nodes
    // A leaf node needs a pointer to the next leaf node
    

    // Update the children of the current node by adding the new Node pointer to the end
    node -> children[ptr_array_size - 1] = newLeafNode;
    // node -> children.clear();
    // node -> children.push_back(newLeafNode);
    // node -> children.shrink_to_fit();

    vector<uint>().swap(node_vector);
    vector<uint>().swap(new_node_vector);

    // update parent node if necessary
    if (node -> parent != nullptr) {
        insertIntoParent(node, newLeafNode -> keys[0], newLeafNode);
    } else {
        // create a new root node
        BPlusTreeNode* newRootNode = createInternalNode();
        newRootNode -> keys[0] = newLeafNode -> keys[0];
        //newRootNode -> keys.push_back(newLeafNode -> keys[0]);
        newRootNode -> children[0] = node;
        newRootNode -> children[1] = newLeafNode;
        // newRootNode -> children.push_back(node);
        // newRootNode -> children.push_back(newLeafNode);
        // newRootNode -> keys.shrink_to_fit();
        // newRootNode -> children.shrink_to_fit();
        //newRootNode -> recordLists.shrink_to_fit();
        root = newRootNode;
        node -> parent = newRootNode;
        newLeafNode -> parent = newRootNode;
    }


    // for (int i = 0; i < newLeafNode->numKeys; i++){
    //     cout << newLeafNode->numKeys << endl;
    // }

    cout << newLeafNode->numKeys << endl;
    // for (int i = 0; i < newLeafNode->numKeys; i++){
    //     cout << newLeafNode->numKeys << endl;
    // }
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
    newInternalNode -> numKeys = 1;
    newInternalNode -> parent = nullptr;
    return newInternalNode;
}

void BPlusTree::insertIntoParent(BPlusTreeNode* left, uint key, BPlusTreeNode* right) {
    // check if parentNode requires splitting since its full
    int key_array_size = sizeof(left->keys)/sizeof(left->keys[0]);
    int ptr_array_size = sizeof(left->children)/sizeof(left->children[0]);

    BPlusTreeNode* parentNode = left -> parent;

    // Chcek if parent node is null (root node)
    if (parentNode == nullptr) {
        // create a new root node
        BPlusTreeNode* newRootNode = createInternalNode();
        newRootNode -> keys[0] = key;
        newRootNode -> children[0] = left;
        newRootNode -> children[1] = right;
        // newRootNode -> children.push_back(left);
        // newRootNode -> children.push_back(right);
        // newRootNode -> keys.shrink_to_fit();
        // newRootNode -> children.shrink_to_fit();
        // newRootNode -> recordLists.shrink_to_fit();
        left -> parent = newRootNode;
        right -> parent = newRootNode;
        root = newRootNode;
        return;
    }


    if (parentNode -> numKeys == maxKeys) {
        splitInternalNode(parentNode, key);
        return;
    }

    
    // find index position to insert new key
    int index = 0;
    while (index < parentNode -> numKeys && key > parentNode -> keys[index]) {
        ++index;
    }

    // Create a vector for parentNode
    vector<uint> parentNode_keys = arrayToVector(parentNode -> keys, parentNode -> numKeys);
    vector<BPlusTreeNode*> parentNode_children = ptrarrayToVector(parentNode -> children, (parentNode -> numKeys) + 1);

    // insert key and pointers at appropriate positions
    parentNode_keys.insert(parentNode_keys.begin() + index, key);
    vectorToArray(parentNode_keys, parentNode->keys, key_array_size);
    //parentNode -> keys.insert(parentNode -> keys.begin() + index, key);
    parentNode_children.insert(parentNode_children.begin() + index + 1, right);
    ptrvectorToArray(parentNode_children, parentNode -> children, ptr_array_size);
    //parentNode -> children.insert(parentNode -> children.begin() + index + 1, right);
    // parentNode -> keys.shrink_to_fit();
    // parentNode -> children.shrink_to_fit();
    // parentNode -> recordLists.shrink_to_fit();
    ++parentNode -> numKeys;

    vector<uint>().swap(parentNode_keys);
    vector<BPlusTreeNode*>().swap(parentNode_children);
}

void BPlusTree::splitInternalNode(BPlusTreeNode* node, uint key) {
    // create new internal node
    BPlusTreeNode* newInternalNode = createInternalNode();

    int key_array_size = sizeof(node->keys)/sizeof(node->keys[0]);
    int ptr_array_size = sizeof(node->children)/sizeof(node->children[0]);

    vector<uint> node_keys = arrayToVector(node -> keys, node -> numKeys);
    vector<BPlusTreeNode*> node_children = ptrarrayToVector(node -> children, (node -> numKeys) + 1);
    vector<uint> new_node_keys;
    vector<BPlusTreeNode*> new_node_children;

    // move half of keys+pointers to newly created internal node
    int mid = ceil(node -> numKeys) / 2;
    int midKey = node -> keys[mid]; //middle key is inserted into parent node


    // Insert into vector then skip the median
    // 1. Find location to insert key into node_vector
    int index;
    for (int i = 0; i < node->numKeys; i++){
        if (key < node -> keys[i]){
            index = i;
        }
        if (i == node->numKeys - 1){
            index = node->numKeys;
        }
    }

    // 2. Split and assign to both original node and new vector
    node_keys.insert(node_keys.begin() + index, key);
    new_node_keys.assign(node_keys.begin() + mid + 1, node_keys.end());
    vectorToArray(new_node_keys, newInternalNode -> keys, key_array_size);
    new_node_children.assign(node_children.begin() + mid + 1, node_children.end());
    ptrvectorToArray(new_node_children, newInternalNode -> children, ptr_array_size);
    node_keys.resize(mid);
    node_children.resize(mid + 1);
    vectorToArray(node_keys, node -> keys, key_array_size);
    ptrvectorToArray(node_children, node -> children, ptr_array_size);

    //newInternalNode -> keys.assign(node -> keys.begin() + mid + 1, node -> keys.end());
    // new_node_keys.assign(node_keys.begin() + mid + 1, node_keys.end());
    // vectorToArray(new_node_keys, newInternalNode -> keys, key_array_size);

    //newInternalNode -> children.assign(node -> children.begin() + mid + 1, node -> children.end());
    // new_node_children.assign(node_children.begin() + mid + 1, node_children.end());
    // ptrvectorToArray(new_node_children, newInternalNode -> children, ptr_array_size);

    // newInternalNode -> keys.shrink_to_fit();
    // newInternalNode -> children.shrink_to_fit();
    // newInternalNode -> recordLists.shrink_to_fit();
    newInternalNode -> numKeys = floor(node -> numKeys / 2);
    node -> numKeys = mid;

    // update pointers
    newInternalNode -> parent = node -> parent;
    for (int i = 0; i < newInternalNode -> numKeys + 1; i++){
        BPlusTreeNode* child_node = newInternalNode -> children[i];
        child_node -> parent = newInternalNode;
    }
    // for (auto child : newInternalNode -> children) {
    //     child -> parent = newInternalNode;
    // }

    // delete all vectors
    vector<uint>().swap(node_keys);
    vector<BPlusTreeNode*>().swap(node_children);
    vector<uint>().swap(new_node_keys);
    vector<BPlusTreeNode*>().swap(new_node_children);

    insertIntoParent(node, midKey, newInternalNode);
}

// Remove/Delete a key from the BPlusTree
void BPlusTree::remove(uint key){

    // 1. Search for the key, if it doesn't exist, just print not found and leave
    BPlusTreeNode* target_node;
    bool in_index;

    tie(target_node, in_index) = search_to_delete(key);

    if (!(in_index)){
        cout << "Record not found! " << endl;
        return;
    }
    
    cout << "Deleting key " << endl;

    // Start a recursion loop - the exit points are from points 3 to 6, 7 will update target_node to its parent
    while (target_node != root){
        // 2. Remove the entry in the node
        int pos;
        bool key_found;
        int key_array_size = sizeof(target_node->keys)/sizeof(target_node->keys[0]);

        tie(pos, key_found) = search_key_in_node(target_node, key);

        vector<uint> key_vector = arrayToVector(target_node->keys, target_node->numKeys);

        // if found, then delete everything related to the key
        if (key_found){
            target_node -> numKeys--;
            target_node -> recordLists.erase(target_node -> recordLists.begin() + pos);

            // Keys need to be shifted forwards
            key_vector.erase(key_vector.begin() + pos);
            vectorToArray(key_vector, target_node->keys, key_array_size);
        }

        vector<uint>().swap(key_vector);

        // 3. Check if the leaf node has underflowed (basically doesn't meet the min. key requirement)
        // If it is found to be balanced even after deletion, then update the parent node
        if (leaf_is_balanced(target_node)){
            remove_deleted_keys(target_node, key);
            return;
        }

        // 4. Then, check if it is possible to borrow keys from sibling nodes, if yes, do so and return.
        if (borrowSiblings(target_node)){
            remove_deleted_keys(target_node, key);
            return;
        }

        // 5. Else, merge with sibling node - if merge is done, then we need to update the parent and continue running the loop
        BPlusTreeNode* parent = target_node -> parent;
        int nodeIndex;
            for (int i = 0; i < parent -> numKeys; i++) {
                if (parent -> children[i] == target_node) {
                    nodeIndex = i;
                }
            }
    
        // find leftSibling and check whether it exists
        BPlusTreeNode* leftSibling = parent -> children[nodeIndex - 1];
        BPlusTreeNode* rightSibling = parent -> children[nodeIndex + 1];
        if (leftSibling != nullptr){
            mergeNodes(leftSibling, target_node);
        }
        else{
            mergeNodes(target_node, rightSibling);
        }

        // 6. Update parent node - after every merge, need to update the target_node
        target_node = target_node -> parent;
    }

    // 3. If it is root, collapse root and return, this scenario occurs when the children of the root node merge, leaving only one child
    if (target_node == root){
        if (target_node -> numKeys < minKeys){
            root = target_node -> children[0];
        }
    }

}

// Remove all instances of a key from the index and also updates them to their smallest right subtree
void BPlusTree::remove_deleted_keys(BPlusTreeNode* target_node, uint key){

    BPlusTreeNode* cursor = target_node;
    int pos;
    bool key_found;

    // Keep visiting each parent until you reach the root, deleting every same key that remains in the index
    while(cursor != root){
        tie(pos, key_found) = search_key_in_node(cursor, key);
        if (key_found){
            uint replacement_key = find_smallest_right_subtree(cursor -> children[pos + 1]);
            cursor -> keys[pos] = replacement_key;
        }
        cursor = cursor -> parent;
    }

    // When it reaches root, do one last time
    tie(pos, key_found) = search_key_in_node(cursor, key);
    if (key_found){
        uint replacement_key = find_smallest_right_subtree(cursor -> children[pos + 1]);
        cursor -> keys[pos] = replacement_key;
    }

}

void BPlusTree::mergeNodes(BPlusTreeNode* left, BPlusTreeNode* right) {
    // transfer everything from right node to the left node
    // transfer keys
    int leftStartIndex = left -> numKeys - 1;
    for (int i = 0; i < right -> numKeys; i++) {
        left -> keys[leftStartIndex + i] = right -> keys[i];
    }
    if (left -> isLeafNode && right -> isLeafNode) {
        // transfer recordLists
        left -> recordLists.insert(left -> recordLists.begin() + leftStartIndex, right -> recordLists.begin(), right -> recordLists.end());
    } else {
        // transfer children
        for (int i = 0; i < 24; i++) {
            left -> children[leftStartIndex + i] = right -> children[i];
        }
    }
    // update numKeys for left
    left -> numKeys += right -> numKeys;
    
    // handle parent
    BPlusTreeNode* parentNode = right -> parent;
    // delete pointer to right
    for (BPlusTreeNode* child : parentNode -> children) {
        if (child == right) {
            child = nullptr;
        }
    }
    
    // handle parent indices... im stuck here
    // get all indices of 
    // delete the index between the left and right nodes
    uint key = right -> keys[0];
    int pos;
    bool is_exists;
    tie(pos, is_exists) = search_key_in_node(parentNode, key);

    parentNode -> numKeys--;
    parentNode -> recordLists.erase(parentNode -> recordLists.begin() + pos);

    int key_array_size = sizeof(parentNode -> keys)/sizeof(parentNode -> keys[0]);

    // Keys need to be shifted forwards
    vector<uint> key_vector = arrayToVector(parentNode -> keys, parentNode -> numKeys);
    key_vector.erase(key_vector.begin() + pos);
    vectorToArray(key_vector, parentNode->keys, key_array_size);
    
    // delete right node
    delete right;
}

bool BPlusTree::borrowSiblings(BPlusTreeNode* node) {
    BPlusTreeNode* parent = node -> parent;
    
    // iterate through parent to find index of this node
    int nodeIndex;
    for (int i = 0; i < parent -> numKeys; i++) {
        if (parent -> children[i] == node) {
            nodeIndex = i;
        }
    }
    
    // find leftSibling and check whether it exists
    BPlusTreeNode* leftSibling = parent -> children[nodeIndex - 1];
    if (leftSibling != nullptr) {
        // check whether leftSibling can lend us a key
        if (leftSibling -> numKeys > minKeys) {
            // take the last key and corresponding recordList from leftSibling
            int borrowIndex = leftSibling -> numKeys - 1;
            int borrowedKey = leftSibling -> keys[borrowIndex];
            vector<Record*> borrowedRecordList = leftSibling -> recordLists.back();
            
            // erase the last key and corresponding recordList from leftSibling
            leftSibling -> keys[borrowIndex] = 0;
            leftSibling -> recordLists.pop_back();
            
            // update leftSibling numKeys
            leftSibling -> numKeys--;
            
            // insert the stuff into front of node
            // make space at front of keys array
            for (int i = node -> numKeys; i > 0; i--) {
                node -> keys[i + 1] = node -> keys[i];
            }
            // insert borrowedKey and borrowedRecordList into node
            node -> keys[0] = borrowedKey;
            node -> recordLists.insert(node -> recordLists.begin(), borrowedRecordList);
            
            // update node numKeys
            node -> numKeys++;
            
            // does anything have to be done for the parent?
            // Should update parent

            
            //
            return true;
        }
    }
    
    // find rightSibling and check whether it exists
    BPlusTreeNode* rightSibling = parent -> children[nodeIndex + 1];
    if (rightSibling != nullptr) {
        // check whether rightSibling can ledn us a key
        if (rightSibling -> numKeys > minKeys) {
            // take the first key and corresponding recordList from rightSibling
            int borrowedKey = rightSibling -> keys[0];
            vector<Record*> borrowedRecordList = rightSibling -> recordLists.front();
            
            // erase the first key and corresponding recordList from rightSibling
            for (int i = 0; i < rightSibling -> numKeys; i++) {
                rightSibling -> keys[i] = rightSibling -> keys[i + 1];
            }
            rightSibling -> recordLists.erase(rightSibling -> recordLists.begin());
            
            // update rightSibling numKeys
            rightSibling -> numKeys--;
            
            // inserts stuff into back of node
            int insertIndex = node -> numKeys - 1;
            node -> keys[insertIndex] = borrowedKey;
            node -> recordLists.push_back(borrowedRecordList);
            
            // update node numKeys
            node -> numKeys++;
            
            // does anything have to be done for the parent?
            
            //
            return true;
        }
    }
    
    return false;
}

tuple<BPlusTreeNode*, BPlusTreeNode*>findSiblings(BPlusTreeNode* target_node){
    
    // Go to parent of this node and search for the first key of the target node
    BPlusTreeNode* parentNode = target_node -> parent;


}

// Searches for an index of a key within a node
tuple<int, bool> BPlusTree::search_key_in_node(BPlusTreeNode* target_node, uint key){

    for (int i = 0; i < target_node -> numKeys; i++){
        if (key == target_node -> keys[i]){
            return make_tuple(i, true);
        }
    }
    return make_tuple(0, false);
}

// Given a key, find whether it exists in the BPlusTree, if it does, return the node pointer to that key
tuple<BPlusTreeNode*, bool> BPlusTree::search_to_delete(uint key){

    BPlusTreeNode* cursor = root;

    while(!(cursor->isLeafNode)){
        int index;
        for (int i = 0; i < cursor -> numKeys; i++){
            if (key < cursor -> keys[i]){
                index = i;
                break;
            }
            if (i == cursor -> numKeys - 1){
                index = cursor -> numKeys;
            }
        }
        cursor = cursor -> children[index];
    }

    for (int j = 0 ; j < cursor -> numKeys; j++){
        if (key == cursor -> keys[j]){
            return make_tuple(cursor, true);
        }
    }
    return make_tuple(nullptr, false);
}

// Follow the right ptr, then follow all the first children until the leaf node
uint BPlusTree::find_smallest_right_subtree(BPlusTreeNode* right_ptr){

    BPlusTreeNode* cursor = root;

    while(!(cursor->isLeafNode)){
        cursor = cursor -> children[0];
    }

    return cursor -> keys[0];

}

// Given a leaf node, check whether it is balanced
bool BPlusTree::leaf_is_balanced(BPlusTreeNode* leaf_node){

    if (leaf_node -> numKeys < minKeys){
        return false;
    }
    else{ return true;}
    
}

// Given an internal node, check whether it is balanced
bool BPlusTree::internal_is_balanced(BPlusTreeNode* internal_node){

    if (internal_node -> numKeys < floor(order - 1 / 2)){
        return false;
    }
    else{ return true;}

}

// void BPlusTree::remove(int key) {
//     // call recursive fn to delete key
//     removeRecursive(root, key);
// }

// void BPlusTree::removeRecursive(BPlusTreeNode* node, int key) {
//     if (node == nullptr) {
//         return;
//     }

//     vector<uint> node_keys = arrayToVector(node -> keys, node -> numKeys);
//     if (node -> isLeafNode){
//         vector<BPlusTreeNode*> node_children = ptrarrayToVector(node -> children, maxKeys + 1);
//         node_children.erase(node_children.begin(), node_children.end() - 2);
//     }
//     else{
//         vector<BPlusTreeNode*> node_children = ptrarrayToVector(node -> children, (node -> numKeys) + 1);
//     }
    

//     // find position of key in node
//     int index = 0;
//     while (index < node -> numKeys && key > node -> keys[index]) {
//         ++index;
//     }

//     if (index < node -> numKeys && key == node -> keys[index]) {
//         // key found in this node
//         if (node -> isLeafNode) {
//             // key was found in a leaf node
//             // delete the key and the record from the leaf node
//             //node -> keys.erase(node -> keys.begin() + index);
//             node_keys.erase(node_keys.begin() + index);


//             node -> recordLists.erase(node -> recordLists.begin() + index);
//             --node -> numKeys;
//         } else {
//             // key was found in an internal node
//             // replace the key w predecessor/successor key from a leaf node
//             BPlusTreeNode* leftChildNode = node -> children[index];
//             BPlusTreeNode* rightChildNode = node -> children[index + 1];

//             // find predecessor key from left child node
//             while (!leftChildNode -> isLeafNode) {
//                 leftChildNode = leftChildNode -> children.back();
//             }
//             int precedecessorKey = leftChildNode -> keys.back();

//             // find the successor key from the right child node
//             while (!rightChildNode -> isLeafNode) {
//                 rightChildNode = rightChildNode -> children.front();
//             }
//             int successorKey = rightChildNode -> keys.front();

//             // choose between the predecessor and successor keys to replace the deleted key
//             int replacementKey;
//             if (node -> keys.size() - index <= index) {
//                 replacementKey = precedecessorKey;
//             } else {
//                 replacementKey = successorKey;
//             }

//             // replace the key in the internal node
//             node -> keys[index] = replacementKey;

//             // recursively delete the key from the leaf node
//             removeRecursive((replacementKey == precedecessorKey) ? leftChildNode : rightChildNode, replacementKey);
//         }
//     } else {
//         // key not found in this node, need to continue searching in appropriate child node
//         removeRecursive(node -> children[index], key);
//     }
// }

// BPlusTreeNode* findSiblingNode(BPlusTreeNode* node) {
//     if (node -> parent == nullptr) {
//         return nullptr; // rootNode has no sibling
//     }

//     BPlusTreeNode* parentNode = node -> parent;
    
//     int index = 0;
//     while (index < parentNode -> children.size() && parentNode -> children[index] != node) {
//         ++index;
//     }

//     // check if node has left sibling
//     if (index > 0) {
//         return parentNode -> children[index - 1];
//     }

//     // check if node has right sibling
//     if (index < parentNode -> children.size() - 1) {
//         return parentNode -> children[index + 1];
//     }

//     return nullptr; // the case of node having no siblings
// }

// void BPlusTree::mergeNodes(BPlusTreeNode* left, BPlusTreeNode* right) {
//     // find the parent node
//     BPlusTreeNode* parentNode = left -> parent;

//     // find index of right node in the parent node
//     int index = 0;
//     while (index < parentNode -> children.size() && parentNode -> children[index] != right) {
//         ++index;
//     }

//     // shift the keys and the children from the right node to the left node
//     left -> keys.push_back(parentNode -> keys[index - 1]);
//     left -> keys.insert(left -> keys.end(), right -> keys.begin(), right -> keys.end());
//     left -> children.insert(left -> children.end(), right -> children.begin(), right -> children.end());

//     // update parent nodes keys and children after the shift
//     parentNode -> keys.erase(parentNode -> keys.begin() + index - 1);
//     parentNode -> children.erase(parentNode -> children.begin() + index);

//     // update right node children's parent pointer post shift
//     for (auto child : right -> children) {
//         child -> parent = left;
//     }

//     // delete right node
//     delete right;
// }

// void redistributeKeys(BPlusTreeNode* left, BPlusTreeNode* right) {
//     // find parent node
//     BPlusTreeNode* parentNode = left -> parent;

//     // find index of right node in parent node
//     int index = 0;
//     while (index < parentNode -> children.size() && parentNode -> children[index] != right) {
//         ++index;
//     }

//     // redistribute keys between left and right nodes
//     if (left -> keys.size() < right -> keys.size()) {
//         // redistribute from right to left since right has more keys
//         int redistributionIndex = right -> keys.size() / 2;
//         left -> keys.push_back(parentNode -> keys[index - 1]);
//         parentNode -> keys[index - 1] = right -> keys[redistributionIndex];
//         left -> keys.insert(left -> keys.end(), right -> keys.begin(), right -> keys.begin() + redistributionIndex);
//         right -> keys.erase(right -> keys.begin(), right -> keys.begin() + redistributionIndex);

//         // check if nodes are internal nodes, because then they require children pointers to be redistributed too
//         if (!left -> isLeafNode) {
//             left -> children.insert(left -> children.end(), right -> children.begin(), right -> children.begin() + redistributionIndex + 1);
//             right -> children.erase(right -> children.begin(), right -> children.begin() + redistributionIndex + 1);
//         }
//     } else {
//         // redistribute keys from left to right since left has more keys
//         int redistributionIndex = left -> keys.size() / 2;
//         right -> keys.insert(right -> keys.begin(), parentNode -> keys[index - 1]);
//         parentNode -> keys[index - 1] = left -> keys[redistributionIndex - 1];
//         right -> keys.insert(right -> keys.begin(), left -> keys.begin() + redistributionIndex - 1, left -> keys.end());
//         left -> keys.erase(left -> keys.begin() + redistributionIndex - 1, left -> keys.end());

//         // check if nodes are internal nodes, because then they require children pointers' redistribution as well
//         if (!left -> isLeafNode) {
//             right -> children.insert(right -> children.begin(), left -> children.begin() + redistributionIndex, left -> children.end());
//             left -> children.erase(left -> children.begin() + redistributionIndex, left -> children.end());
//         }
//     }
// }

void BPlusTree::print_tree(){

    BPlusTreeNode* first_leaf_node;
    bool is_duplicate;

    tie(first_leaf_node, is_duplicate) = search_key(0);

    cout << "Here1" << endl;

    BPlusTreeNode* target_node = first_leaf_node;

    cout << "Here2" << endl;

    int ptr_array_size = sizeof(target_node -> children)/sizeof(target_node -> children[0]);

    cout << "Here3" << endl;

    int node_counter = 1;

    while(target_node -> children[ptr_array_size - 1] != nullptr){
        cout << "Leaf Node " << node_counter << endl << endl;
        for (int i = 0; i < target_node -> numKeys ; i++){
            cout << target_node -> keys[i] << endl;
        }
        target_node = target_node -> children[ptr_array_size - 1];
        node_counter++;
    }

    // while(target_node -> children[ptr_array_size - 1] != nullptr){
    //     cout << "Here4" << endl;
    //     for (int i = 0; i < target_node -> numKeys; ++i) {
    //         cout << target_node -> keys[i] << ": ";
    //         for (int j = 0; j < target_node -> recordLists[i].size(); ++j) {
    //             Record* recordToPrint = target_node -> recordLists[i][j];
    //             cout << "(" << recordToPrint -> tconst << ", " << recordToPrint -> averageRating << ", " << recordToPrint -> numVotes << ") ";
    //         }
    //         cout << " | ";
    // }
    // }
}

void BPlusTree::printEntireBPlusTree() {
    cout << "now printing BPlusTree" << endl;
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

    cout << "Leaf Node: " << endl;

    if (treeRoot -> isLeafNode) {
        // print keys and values of leaf node
        for (int i = 0; i < treeRoot -> numKeys; i++) {
            cout << endl << endl;
            cout << treeRoot -> keys[i] << ": ";
            for (int j = 0; j < treeRoot -> recordLists[i].size(); j++) {
                Record* recordToPrint = treeRoot -> recordLists[i][j];
                cout << "(" << recordToPrint -> tconst << ", " << recordToPrint -> averageRating << ", " << recordToPrint -> numVotes << ") ";
            }
            cout << " | " << endl;
        }
    } else {
        // recursively print child nodes
        for (int i = 0; i < treeRoot -> numKeys + 1; ++i) {
            printBPlusTree(treeRoot -> children[i]);
        }
        //printBPlusTree(treeRoot -> children[treeRoot -> numKeys]);
    }
}

void BPlusTree::print_root_node() {
    cout << "Keys in the root node: " << endl;
    for (int i = 0; i < root->numKeys; i++){
        cout << root->keys[i] << endl; 
    }
}

// Function to count the number of nodes in the tree
int BPlusTree::countNodes(BPlusTreeNode* root) {

    int nodes = 0;
    std::queue<BPlusTreeNode*> q;
    q.push(root);

    while (!q.empty()) {
        BPlusTreeNode* current = q.front();
        q.pop();
        nodes++;

        if (!current->isLeafNode) {
            for (size_t i = 0; i <= current->numKeys; ++i) {
                if (current->children[i] != nullptr)
                    q.push(current->children[i]);
            }
        }
    }

    return nodes;
}


// Function to count the number of levels in the tree
int BPlusTree::countLevels(BPlusTreeNode* root) {
    
    BPlusTreeNode* cursor = root;

    uint key = 0;
    int num_levels = 1;

    while (!(cursor -> isLeafNode)){
        cursor = cursor -> children[0];
        num_levels++;
    }

    return num_levels;
}

void BPlusTree::print_number_of_nodes_and_levels(){

    BPlusTreeNode* cursor = root;
    int number_of_nodes = 1;
    int number_of_levels = 1;

    for (int i = 0; i < cursor -> numKeys + 1; i++){
        if (cursor -> isLeafNode){

        }
        else{
            number_of_levels++;
        }

    }

};