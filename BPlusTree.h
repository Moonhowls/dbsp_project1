#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include "Node.h"
#include "disk.h"
#include <tuple>
#include <vector>

using namespace std;

typedef tuple<vector<Node*>, bool> searchResult;
typedef unsigned int uint;
typedef unsigned char uchar;
typedef vector<tuple<string, uchar*, int>> keyVector;

class BPlusTree {

    public:
        Node* rootNode;
        unsigned int numNodes;

        BPlusTree();

        // methods
        searchResult searchTargetKey(uint targetKey);
        Node* insertTargetKey(vector<Node*> ancestryTree, Record targetRecord, uchar* blockAddress, int offset, bool isDuplicate);
        vector<Node*> searchKeyRange(uint lowerBound, uint upperBound);
        bool deleteTargetKey(vector<Node*> ancestryTree, uint targetKey);
};

#endif /* BPLUSTREE_H */