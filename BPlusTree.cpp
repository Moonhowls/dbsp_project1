#include "BPlusTree.h"
#include <queue>


/**
 * @brief 
 * 
 * @param arr 
 * @param size 
 * @return std::vector<uint> 
 */
std::vector<uint> arrayToVector(uint arr[], int size) {
    std::vector<uint> vec(arr, arr + size);
    return vec;
}

/**
 * @brief Converts a vector to an array
 * 
 * @param vec 
 * @param arr 
 * @param size 
 */
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

/**
 * @brief 
 * 
 * @param arr 
 * @param size 
 * @return std::vector<BPlusTreeNode*> 
 */
std::vector<BPlusTreeNode*> ptrarrayToVector(BPlusTreeNode* arr[], int size) {
    std::vector<BPlusTreeNode*> vec(arr, arr + size);
    return vec;
}

/**
 * @brief 
 * 
 * @param vec 
 * @param arr 
 * @param size 
 */
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
        for (int i = 0; i < node -> numKeys + 1; ++i) {
            destroyTree(node -> children[i]);
        }
    }

    delete node;
}

// Searches the BPlusTree and returns:
// BPlusTreeNode* : a node pointer if a target is found
// bool: whether the key is a duplicate

/**
 * @brief Searches the BPlusTree to return to us a pointer to the node (if found), and a bool indicating if the searched for key has duplicates
 * 
 * @param key : the key we're searching the tree for
 * @return tuple<BPlusTreeNode*, bool> 
 */
tuple<BPlusTreeNode*, bool> BPlusTree::search(uint key) {
    return searchRecursive(root, key);
}

/**
 * @brief 
 * 
 * @param key 
 * @return tuple<BPlusTreeNode*, bool> 
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

/**
 * @brief Given a range of keys, finds and returns a vector of associated Record pointers and the no. of index nodes accessed
 * 
 * @param lower_bound 
 * @param upper_bound 
 * @return tuple<vector<Record*>, int> 
 */
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
}

/**
 * @brief Helper function to assist with searching through a BPlus tree
 * 
 * @param node 
 * @param key 
 * @return tuple<BPlusTreeNode*, bool> 
 */
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

/**
 * @brief The main function for inserting a key and its associated record into the tree
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
 * @brief Helper function for insertion
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

/**
 * @brief Helper function for insertion
 * 
 * @param node 
 * @param key 
 * @param record 
 */
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

    // Inserting into a non-full leaf
    node -> recordLists.insert(node -> recordLists.begin() + index, vector<Record*>{record});

    ++node -> numKeys;

    // Deallocate memory for keys
    vector<uint>().swap(key_vector);
}

/**
 * @brief Assists in splitting a leaf node during the insertion process
 * 
 * @param node 
 * @param key 
 * @param record 
 */
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
    node -> recordLists.insert(node -> recordLists.begin() + index, vector<Record*>{record});
    newLeafNode -> recordLists.assign(node -> recordLists.begin() + mid, node -> recordLists.end());
    node -> recordLists.resize(mid);

    newLeafNode -> numKeys = node -> numKeys - mid;
    node -> numKeys = mid;

    // update pointers accordingly
    newLeafNode -> parent = node -> parent;
    newLeafNode -> isLeafNode = true;

    // Update newLeafNode with the ptr to the next leaf
    if (node->children[ptr_array_size - 1] != nullptr){
        newLeafNode -> children[ptr_array_size - 1] = node -> children[ptr_array_size - 1];
    }
    
    // Update the children of the current node by adding the new Node pointer to the end
    node -> children[ptr_array_size - 1] = newLeafNode;

    vector<uint>().swap(node_vector);
    vector<uint>().swap(new_node_vector);

    // update parent node if necessary
    if (node -> parent != nullptr) {
        insertIntoParent(node, newLeafNode -> keys[0], newLeafNode);
    } else {
        // create a new root node
        BPlusTreeNode* newRootNode = createInternalNode();
        newRootNode -> keys[0] = newLeafNode -> keys[0];
        newRootNode -> children[0] = node;
        newRootNode -> children[1] = newLeafNode;
        root = newRootNode;
        node -> parent = newRootNode;
        newLeafNode -> parent = newRootNode;
    }

}

/**
 * @brief Creates a leaf node
 * 
 * @return BPlusTreeNode* 
 */
BPlusTreeNode* BPlusTree::createLeafNode() {
    BPlusTreeNode* newLeafNode = new BPlusTreeNode;

    newLeafNode -> isLeafNode = true;
    newLeafNode -> numKeys = 0;
    newLeafNode -> parent = nullptr;
    return newLeafNode;
}

/**
 * @brief Creates a new internal node
 * 
 * @return BPlusTreeNode* 
 */
BPlusTreeNode* BPlusTree::createInternalNode() {
    BPlusTreeNode* newInternalNode = new BPlusTreeNode;

    newInternalNode -> isLeafNode = false;
    newInternalNode -> numKeys = 1;
    newInternalNode -> parent = nullptr;
    return newInternalNode;
}

/**
 * @brief Helper function to assist with insertion into a parent nod e in the insertion process
 * 
 * @param left 
 * @param key 
 * @param right 
 */
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
    parentNode_children.insert(parentNode_children.begin() + index + 1, right);
    ptrvectorToArray(parentNode_children, parentNode -> children, ptr_array_size);
    ++parentNode -> numKeys;

    vector<uint>().swap(parentNode_keys);
    vector<BPlusTreeNode*>().swap(parentNode_children);
}

/**
 * @brief Splits an internal node if rqeuired during the insertion process to maintain B+ tree properties
 * 
 * @param node 
 * @param key 
 */
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

    newInternalNode -> numKeys = floor(node -> numKeys / 2);
    node -> numKeys = mid;

    // update pointers
    newInternalNode -> parent = node -> parent;
    for (int i = 0; i < newInternalNode -> numKeys + 1; i++){
        BPlusTreeNode* child_node = newInternalNode -> children[i];
        child_node -> parent = newInternalNode;
    }
    
    // delete all vectors
    vector<uint>().swap(node_keys);
    vector<BPlusTreeNode*>().swap(node_children);
    vector<uint>().swap(new_node_keys);
    vector<BPlusTreeNode*>().swap(new_node_children);

    insertIntoParent(node, midKey, newInternalNode);
}

/**
 * @brief Deletes a key from the BPlus tree
 * 
 * @param key 
 */
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

/**
 * @brief Removes all instances of a key from the index + updates them to their smallest right subtree
 * 
 * @param target_node 
 * @param key 
 */
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

/**
 * @brief Handles the merging of 2 nodes, required in underflow scenarios of the deletion process
 * 
 * @param left 
 * @param right 
 */
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
    
    // delete the index between the left and right nodes to handle parent indices properly
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

/**
 * @brief Handles functionality of borrowing keys from sibling nodes in underflow scenarios
 * 
 * @param node 
 * @return true 
 * @return false 
 */
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
            
            // update parent to be new first key of current node
            parent -> keys[nodeIndex - 1] = node -> keys[0];
            
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
            
            // update parent to be new first key of right sibling
            parent -> keys[nodeIndex] = rightSibling -> keys[0];
            
            //
            return true;
        }
    }
    
    return false;
}


// Searches for an index of a key within a node
/**
 * @brief Searches within a node to find the index at which a particular key is located (for deletion functionality)
 * 
 * @param target_node 
 * @param key 
 * @return tuple<int, bool> 
 */
tuple<int, bool> BPlusTree::search_key_in_node(BPlusTreeNode* target_node, uint key){

    for (int i = 0; i < target_node -> numKeys; i++){
        if (key == target_node -> keys[i]){
            return make_tuple(i, true);
        }
    }
    return make_tuple(0, false);
}

/**
 * @brief Given a key, checks for its existence in the B+ tree for deletion purposes, returns pointer to node containing that key if it exists
 * 
 * @param key 
 * @return tuple<BPlusTreeNode*, bool> 
 */
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

/**
 * @brief Helper function that simply follows the righrt pointer and then follows subsequent first children until the leaf node is reached
 * 
 * @param right_ptr 
 * @return uint 
 */
uint BPlusTree::find_smallest_right_subtree(BPlusTreeNode* right_ptr){

    BPlusTreeNode* cursor = right_ptr;

    while(!(cursor->isLeafNode)){
        cursor = cursor -> children[0];
    }

    return cursor -> keys[0];

}

/**
 * @brief Checks whether a leaf node is balanced or not
 * 
 * @param leaf_node 
 * @return true 
 * @return false 
 */
bool BPlusTree::leaf_is_balanced(BPlusTreeNode* leaf_node){

    if (leaf_node -> numKeys < minKeys){
        return false;
    }
    else{ return true;}
    
}

/**
 * @brief Checks whether an internal node is balanced or not
 * 
 * @param internal_node 
 * @return true 
 * @return false 
 */
bool BPlusTree::internal_is_balanced(BPlusTreeNode* internal_node){

    if (internal_node -> numKeys < floor(order - 1 / 2)){
        return false;
    }
    else{ return true;}

}

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

}

/**
 * @brief Main function for printing the BPlus Tree
 * 
 */
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

/**
 * @brief Helper function for printing the BPlusTree
 * 
 * @param treeRoot 
 */
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

/**
 * @brief Helps to print the contents of the root node of the tree
 * 
 */
void BPlusTree::print_root_node() {
    cout << "Keys in the root node: " << endl;
    for (int i = 0; i < root->numKeys; i++){
        cout << root->keys[i] << endl; 
    }
}

/**
 * @brief Counts the number of nodes in the tree
 * 
 * @param root 
 * @return int 
 */
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
/**
 * @brief Counts the number of levels in the tree
 * 
 * @param root 
 * @return int 
 */
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