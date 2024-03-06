#include "BPlusTree.h"

BPlusTree::BPlusTree() : rootNode(new Node(true, 0)), numNodes(1) {}

searchResult BPlusTree::searchTargetKey(uint targetKey) {
    
    vector<Node*> searchPath;
    bool keyExists;

    // 1: Set cursor to root node, add root node to search path
    Node* cursor = rootNode;

    searchPath.insert(searchPath.begin(), cursor);

    // 2: While cursor isn't a leaf node find correct pointer to follow by iterating through keys of node
    while (!(cursor -> isLeafNode)) {
        for (int i = 0; i < cursor -> numOfKeys; i++) {
            if (targetKey < cursor -> keyPointerPairs[i].key) {
                cursor = (Node*) cursor -> keyPointerPairs[i].ptr;
                searchPath.insert(searchPath.begin(), cursor);
                break;
            }

            cursor = cursor -> lastPointer;

            searchPath.insert(searchPath.begin(), cursor);
        }
    }

    // 3: cursor = leafNode, search for the kexact key and return whether or not it exists in the B+ Tree index
    for (int i = 0; i < cursor -> numOfKeys; i++) {
        if (targetKey == cursor -> keyPointerPairs[i].key) {
            keyExists = true;
            break;
        }

        keyExists = false;
    }

    // 4: return search path & keyExists in a tuple
    searchResult returnTuple = make_tuple(searchPath, keyExists);
    return returnTuple;
}

Node* BPlusTree::insertTargetKey(vector<Node*> ancestryTree, Record targetRecord, uchar* blockAddress, int offset, bool isDuplicate) {
    Node* targetNode = ancestryTree[0];
    uint targetKey = targetRecord.numVotes;
    string targetId = targetRecord.tconst;

    // 1: Handle duplicate, update key vector
    if (isDuplicate) {
        keyVector* targetVectorPtr;
        for (int i = 0; i < targetNode -> numOfKeys; i++) {
            if (targetKey == targetNode -> keyPointerPairs[i].key) {
                targetVectorPtr = (keyVector*) targetNode -> keyPointerPairs[i].ptr;
                for (int j = 0; j < size(*targetVectorPtr); j++) {
                    if (targetId == get<0>((*targetVectorPtr)[j])) {
                        return targetNode;
                    }
                }
            }
        }

        (*targetVectorPtr).push_back(make_tuple(targetId, blockAddress, offset));
        return targetNode;
    }

    // 2: Create vector and KeyPtrPair for target key
    tuple keyVectorTuple = make_tuple(targetId, blockAddress, offset);
    keyVector newKeyVector = keyVector(1, keyVectorTuple);
    keyVector* newKeyVectorPtr = &newKeyVector;

    Node::KeyPtrPair newKeyPtrPair;
    newKeyPtrPair.key = targetKey;
    newKeyPtrPair.ptr = (void*) newKeyVectorPtr;

    // 3: Insert KeyPtrPair into leaf node
    

}