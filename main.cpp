# include <iostream>
# include <cmath>
# include <malloc.h>
# include <tuple>
# include <fstream>
# include <sstream>
# include <string>
# include <vector>
# include "BPlusTree.h"
# include <chrono>

typedef unsigned int uint;
typedef unsigned char uchar;

using namespace std; 

int main(){
    // Read file from tsv
    // Open the TSV file
    ifstream file("data.tsv");
    if (!file.is_open()) {
        cerr << "Error: Unable to open file." << endl;
        return 1;
    }

    // Skip the first line
    string header;
    getline(file, header);

    vector<Record> records; // Vector to store all records read from the file

    string line;
    // Read each line from the file
    while (getline(file, line)) {
        istringstream iss(line);
        string tconst;
        float averageRating;
        uint numVotes;

        // Extract values from the current line
        if (getline(iss, tconst, '\t') && iss >> averageRating && iss >> numVotes) {
            // Create a Record object and store the values
            Record record = Record(false, tconst, averageRating, numVotes);
            // Add the record to the vector
            records.push_back(record);
        } else {
            cerr << "Error: Unable to parse line: " << line << endl;
        }
    }

    Disk memory_disk = Disk(100000000, 200);

    BPlusTree bptree = BPlusTree(24);
    bptree.printEntireBPlusTree();

    cout << sizeof(BPlusTreeNode) << endl;

    //records.resize(23);

    for (auto&& record : records) {
        uchar* record_address;
        int record_offset;
        tie(record_address, record_offset) = memory_disk.write_record_to_disk(record);
        // bptree.insert(record.numVotes, &record);
        // cout << "Insertion complete" << endl;
    }

    // Read first block of data
    uint block_num, num_of_records;
    vector<tuple<uchar*, int>> first_data_block_addresses;
    tie(block_num, num_of_records, first_data_block_addresses) = memory_disk.read_nth_data_block(0);
    
    cout << endl << "--------------------------------------Experiment 1 Results-----------------------------------" << endl;
    cout << "Number of records in data file: " << records.size() << endl;
    cout << "Size of a record: " << sizeof(Record) << " bytes" << endl;
    cout << "Number of records stored in a block: " << num_of_records << endl;
    cout << "Number of blocks used to store data: " << memory_disk.blocks_in_use << endl;
    cout << "---------------------------------------------------------------------------------------------" << endl;

    cout << endl << "---------------------------------Reading first n data blocks-----------------------------------" << endl;
    for (int i = 0; i < 7; i++){
        if (memory_disk.is_block_in_use(i)){
            uint block_num, num_of_records;
            vector<tuple<uchar*, int>> records;
            tie(block_num, num_of_records, records) = memory_disk.read_nth_data_block(i);
            cout << endl << "Block: " << block_num << endl;
            cout << "Number of records in block: " << num_of_records << endl;
            for (auto record_address : records){
                Record record = memory_disk.read_record(get<0>(record_address), get<1>(record_address));
                cout << "tconst: " << record.tconst << ", averageRating: " << record.averageRating << ", numVotes: " << record.numVotes << endl;
            }
        }
    }
    cout << endl << "-----------------------------------------------------------------------------------------------" << endl;

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    using std::chrono::seconds;
    using std::chrono::minutes;
    using std::chrono::hours;

    auto t1 = high_resolution_clock::now();
    
    // function goes here
    cout << "Insertion process complete" << endl; 
    for (int i = 0; i < 5000; i++){
        uint block_id, num_of_elements;
        vector <tuple<uchar*, int>> record_addresses;
        tie(block_id, num_of_elements, record_addresses) = memory_disk.read_nth_data_block(i);
        for (int j = 0; j < record_addresses.size() ; j++){
            cout << "before inserting record" << endl;
            Record* record_ptr = (Record*)(get<0>(record_addresses[j]) + get<1>(record_addresses[j]));
            bptree.insert(record_ptr->numVotes, record_ptr);
        }
    }
    cout << "All records inserted" << endl;

    auto t2 = high_resolution_clock::now();
    bptree.print_root_node();

    /* Getting number of milliseconds as an integer. */
    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    auto sec_int = duration_cast<seconds>(t2 - t1);
    auto min_int = duration_cast<minutes>(t2 - t1);
    auto hour_int = duration_cast<hours>(t2 - t1);

    /* Getting number of milliseconds as a double. */
    duration<double, std::milli> ms_double = t2 - t1;

    std::cout << ms_int.count() << "ms\n";
    std::cout << ms_double.count() << "ms\n";
    std::cout << sec_int.count() << "s\n";
    std::cout << min_int.count() << "mins\n";
    std::cout << hour_int.count() << "h\n";

    // Retrieve records with keys = 500
    BPlusTreeNode* target_node;
    bool is_duplicate;

    tie(target_node, is_duplicate) = bptree.search_key(500);

    for (int i = 0; i < target_node -> numKeys; i++){
        if (target_node -> keys[i] == 500){
            for (int j = 0; j < target_node -> recordLists[i].size(); j++){
                Record* record_ptr = target_node->recordLists[i][j];
                cout << "Record " << j + 1 << " >>> " << "tconst: " << record_ptr->tconst <<  ", NumVotes: " << record_ptr->numVotes << ", averageRating: " << record_ptr->averageRating << endl;
            }
        }
    }


    // bptree.printEntireBPlusTree();
    return 0;
}
    