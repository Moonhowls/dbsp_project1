# include <iostream>
# include <cmath>
# include <malloc.h>
# include <tuple>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
# include "disk.h"

using namespace std;

typedef unsigned int uint;
typedef unsigned char uchar;

Record::Record(bool deleted, string tconst, float averageRating, uint numVotes){
    this->deleted = deleted;
    this->tconst = tconst;
    this->averageRating = averageRating;
    this->numVotes = numVotes;
};

BlockHeader::BlockHeader(uint block_id, uint num_of_elements){
    this->block_id = block_id;
    this->num_of_elements = num_of_elements;
};

// Disk class replicates the behaviour of disk storage in a disk, but on main memory
Disk::Disk(uint disk_size,uint block_size){
        this->disk_pointer = new uchar[disk_size];
        first_avail_block_pointer = disk_pointer;
        this->disk_size = disk_size;
        this->block_size = block_size;
        blocks_in_use = 0;
        total_num_of_blocks = floor(disk_size / block_size);
    };

Disk::~Disk(){
    delete disk_pointer;
    disk_pointer = nullptr;
}

// Check if block is in use
bool Disk::is_block_in_use(uint block_num){
    if (block_num + 1 > blocks_in_use){
        cout << "This block is not in use." << endl;
        return false;
    }
    return true;
};

    // Returns the start address of the nth data block (start at block 0)
tuple<uint, uint, vector <tuple<uchar*, int>>> Disk::read_nth_data_block(uint block_num){

        vector <Record> records;
        vector <tuple<uchar*, int>> record_addresses;
        tuple<uint, uint, vector<Record>> block_data;
        uint block_id, num_of_elements;

        uchar* block_pointer = disk_pointer + (block_size * block_num);
        BlockHeader* block_header = (BlockHeader*)block_pointer;
        block_id = block_header->block_id;
        num_of_elements = block_header->num_of_elements;
        ++block_header;

        Record* first_record_pointer = (Record*)block_header;
        int max_records_per_block = (block_size - sizeof(BlockHeader)) / sizeof(Record);
        for (int i = 0; i < max_records_per_block; i++){
            if (!(first_record_pointer->deleted)){
                int offset = ((uchar*)first_record_pointer) - block_pointer;
                record_addresses.push_back(make_tuple(block_pointer, offset));
                records.push_back(*first_record_pointer);
                first_record_pointer++;
            }
        }
        return make_tuple(block_id, num_of_elements, record_addresses);
    };

// Reading record given the record address - block address and offset
Record Disk::read_record(uchar* block_address, int offset){
    uchar* record_address = block_address + offset;
    Record* record_pointer = (Record*) record_address;
    return *record_pointer;
};

// Given a block id, find its number of elements (block id starts at 0)
uint Disk::find_num_of_elements_in_block(uint block_id){
    uchar* block_pointer = disk_pointer + (block_id * block_size);
    BlockHeader* block_header = (BlockHeader*) block_pointer;
    return block_header->num_of_elements;
};

// Delete record from disk
// 1. Mark record as deleted
// 2. Add the record address to deleted_vector
void Disk::delete_record_from_disk(uchar* block_address, int offset){
    uchar* deleted_pointer = block_address + offset;
    Record* deleted_record_pointer = (Record*)deleted_pointer;
    (*deleted_record_pointer).deleted = true;
    tuple<uchar*, int> record_address = make_tuple(block_address,offset);
    deleted_chain.push_back(record_address); 
};

// Insert record to disk
// Given a pointer and record, insert the record into the address contained in the pointer
// Returns the memory address right after the record
uchar* Disk::insert_record(Record record, uchar* record_pointer){
    Record* insertion_pointer = (Record*)record_pointer;
    *insertion_pointer = record;
    ++insertion_pointer;
    return (uchar*)insertion_pointer;
};

// Write record into disk, returns the record address
// Record address is a tuple with the block address holding the record plus the 
// relative offset of the record's position within the block
tuple<uchar*, int> Disk::write_record_to_disk(Record target_record){

    tuple<uchar*, int> record_address;
    int offset;
    int max_records_per_block = (block_size - sizeof(BlockHeader)) / sizeof(target_record);
    uchar* insertion_pointer;

    // When writing into empty file, initialise the block header for the first block
    if (blocks_in_use == 0 and (first_avail_block_pointer == disk_pointer)){
        BlockHeader new_block_header = BlockHeader(blocks_in_use, 0);
        BlockHeader* block_pointer = (BlockHeader*)first_avail_block_pointer;
        *block_pointer = new_block_header;
        blocks_in_use++;
    }
    
    // Get number of records in the first available block
    int records_in_cur_block = find_num_of_elements_in_block(blocks_in_use - 1);

    // Find a block to insert into
    // 1. Check if there are deleted slots, if yes, insert into them first
    // Return deleted slot address as record address
    if (deleted_chain.size() != 0){
        tuple<uchar*, int> empty_slot = deleted_chain.front();
        uchar* empty_slot_address= get<0>(empty_slot) + get<1>(empty_slot);
        uchar* insertion_pointer = insert_record(target_record, empty_slot_address);
        uchar* block_pointer = get<0>(empty_slot);
        BlockHeader* block_header = (BlockHeader*)block_pointer;
        block_header->num_of_elements += 1;
        return empty_slot;
    }

    // 2. If no available deleted slots, insert to first_avail_block_pointer
    if (records_in_cur_block < max_records_per_block){ //block not full
        uchar* record_pointer = first_avail_block_pointer + (records_in_cur_block * sizeof(Record)) + sizeof(BlockHeader);
        uchar* insertion_pointer = insert_record(target_record, record_pointer);
        offset = records_in_cur_block * sizeof(Record) + sizeof(BlockHeader);
        BlockHeader* block_header = (BlockHeader*)first_avail_block_pointer;
        block_header->num_of_elements++;
    }
    else{ //block full, go to next block, write block header and then write record
        if (blocks_in_use + 1 >= total_num_of_blocks){ // not enough blocks
            cout << "No more empty blocks left!" << endl;
            exit(-1);
            }
        first_avail_block_pointer = first_avail_block_pointer + block_size;
        BlockHeader new_block_header = BlockHeader(blocks_in_use, 1);
        BlockHeader* block_pointer = (BlockHeader*)first_avail_block_pointer;
        *block_pointer = new_block_header;
        block_pointer++;
        uchar* insertion_pointer = insert_record(target_record, (uchar*)block_pointer);
        blocks_in_use++;
        offset = sizeof(Record) + sizeof(BlockHeader);
    }
    record_address = make_tuple(first_avail_block_pointer, offset);
    return record_address;
};