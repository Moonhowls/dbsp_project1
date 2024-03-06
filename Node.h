#ifndef NODE_H
#define NODE_H

using namespace std;

class Node {

    public:
        struct KeyPtrPair {
            void* ptr = nullptr;
            unsigned int key = 0; // Justification in report, TODO: Insert section

            // not sure if a constructor is necessary here
            // KeyPtrPair() {}
        };

        bool isLeafNode;
        unsigned int numOfKeys;
        KeyPtrPair keyPointerPairs[23]; // Look at report for calculation: n = 23, TODO: find section of report to enter here
        Node* lastPointer;

    public:
        Node(bool isLeafNode, unsigned int numOfKeys);

        // declare any methods below if need be
};

#endif /* NODE_H */