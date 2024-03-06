# include <iostream>
# include <cmath>
# include <tuple>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

typedef unsigned int uint;
typedef unsigned char uchar;

class KeyPtrPair{
public:
    uint key_value;
    vector <string, uchar*> vector_ptr;
};

class Node{
public:
    bool is_leaf;
    KeyPtrPair* key_ptr_pairs; //array of key-pointer pairs
    Node* last_ptr; //leaf: pointer to next leaf node, internal: pointer to last child node
};

class Record{
public:
    bool deleted;
    string tconst;
    float averageRating;
    uint numVotes;
    // size is 36 bytes, word is 4 bytes: 4 + 24 + 4 + 4 = 36
    
    Record(bool deleted, string tconst, float averageRating, uint numVotes);
};


// Each block has a block header, only for blocks that are in use
class BlockHeader{
public:
    uint block_id;
    uint num_of_elements;

    BlockHeader(uint block_id, uint num_of_elements);
};

class Disk {
public:
// Attributes
    // Pointers
    uchar* disk_pointer; //Indicates the start of the disk
    uchar* first_avail_block_pointer; //Indicates the first available block
    
    // Counters
    uint blocks_in_use; //Number of blocks that are in use
    vector<tuple<uchar*, int>> deleted_chain; // Deleted chain of records

    // Constants
    uint disk_size; //Indicates the size of the disk
    uint block_size; //Size of a block
    uint total_num_of_blocks; //Total number of blocks available

//Methods
    //Constructor - constructs a Disk object
    Disk(uint Disk_size,uint Block_size);

    //Destructor - ensures that memory is deallocated
    ~Disk();

    bool is_block_in_use(uint block_num);

    tuple<uint, uint, vector <tuple<uchar*, int>>> read_nth_data_block(uint block_num);

    Record read_record(uchar* block_address, int offset);

    uint find_num_of_elements_in_block(uint block_id);

    void delete_record_from_disk(uchar* block_address, int offset);

    uchar* insert_record(Record record, uchar* record_pointer);

    tuple<uchar*, int> write_record_to_disk(Record target_record);
};