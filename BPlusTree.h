#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include "Node.h"
#include "disk.h"
#include <tuple>
#include <vector>

using namespace std;

typedef tuple<vector<Node*>, bool> searchResult;
typedef unsigned int uint;

class BPlusTree {

    public:
        Node* rootNode = &Node(true, 0);
        unsigned int numNodes = 1;

        BPlusTree();

        // methods
        searchResult searchTargetKey(uint targetKey);
        Node* insertTargetKey(vector<Node*> ancestryTree, Record targetRecord);
        vector<Node*> searchKeyRange(uint lowerBound, uint upperBound);
        bool deleteTargetKey(vector<Node*> ancestryTree, uint targetKey);
};

#endif /* BPLUSTREE_H */