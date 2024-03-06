#ifndef NODE_H
#define NODE_H

using namespace std;

class Node {

    private:
        struct KeyPtrPair {
            void* ptr = nullptr;
            unsigned int key = 0; // should this be 0...?

            // not sure if a constructor is necessary here
            // KeyPtrPair() {}
        };

        bool isLeafNode;
        unsigned int numOfKeys;
        KeyPtrPair keyPointerPairs[5]; // 5 is just a placeholder, assuming we're hardcoding this?
        Node* lastPointer;

    public:
        Node(bool isLeafNode, unsigned int numOfKeys, KeyPtrPair keyPointerPairs, Node* lastPointer);

        // declare any methods below if need be
};

#endif /* NODE_H */