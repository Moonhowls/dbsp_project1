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
# include <set>

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

    int order = 24;

    BPlusTree bptree = BPlusTree(order);
    bptree.printEntireBPlusTree();

    cout << sizeof(BPlusTreeNode) << endl;

    //records.resize(23);

    for (auto&& record : records) {
        uchar* record_address;
        int record_offset;
        tie(record_address, record_offset) = memory_disk.write_record_to_disk(record);
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

    // cout << endl << "---------------------------------Reading first n data blocks-----------------------------------" << endl;
    // for (int i = 0; i < 7; i++){
    //     if (memory_disk.is_block_in_use(i)){
    //         uint block_num, num_of_records;
    //         vector<tuple<uchar*, int>> records;
    //         tie(block_num, num_of_records, records) = memory_disk.read_nth_data_block(i);
    //         cout << endl << "Block: " << block_num << endl;
    //         cout << "Number of records in block: " << num_of_records << endl;
    //         for (auto record_address : records){
    //             Record record = memory_disk.read_record(get<0>(record_address), get<1>(record_address));
    //             cout << "tconst: " << record.tconst << ", averageRating: " << record.averageRating << ", numVotes: " << record.numVotes << endl;
    //         }
    //     }
    // }
    // cout << endl << "-----------------------------------------------------------------------------------------------" << endl;

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    using std::chrono::seconds;
    using std::chrono::minutes;

    auto t1 = high_resolution_clock::now();
    
    // function goes here
    for (int i = 0; i < memory_disk.blocks_in_use; i++){
        uint block_id, num_of_elements;
        vector <tuple<uchar*, int>> record_addresses;
        tie(block_id, num_of_elements, record_addresses) = memory_disk.read_nth_data_block(i);
        for (int j = 0; j < record_addresses.size() ; j++){
            Record* record_ptr = (Record*)(get<0>(record_addresses[j]) + get<1>(record_addresses[j]));
            bptree.insert(record_ptr->numVotes, record_ptr);
        }
    }
    cout << "All records inserted" << endl;

    auto t2 = high_resolution_clock::now();

    cout << endl << "--------------------------------------Experiment 2 Results-----------------------------------" << endl;
    cout << "Parameter n of B+ tree: " << order - 1 << endl;
    cout << "Number of nodes of the B+ tree: " << bptree.countNodes(bptree.root) << endl;
    cout << "Number of levels of the B+ tree: " << bptree.countLevels(bptree.root) << endl;
    bptree.print_root_node();
    cout << "---------------------------------------------------------------------------------------------" << endl;

    // /* Getting number of milliseconds as an integer. */
    // auto ms_int = duration_cast<milliseconds>(t2 - t1);
    // auto sec_int = duration_cast<seconds>(t2 - t1);
    // auto min_int = duration_cast<minutes>(t2 - t1);

    // std::cout << ms_int.count() << "ms\n";
    // std::cout << sec_int.count() << "s\n";
    // std::cout << min_int.count() << "mins\n";

    // // Retrieve records with keys = 500
    // BPlusTreeNode* target_node;
    // bool is_duplicate;

    cout << endl << "--------------------------------------Experiment 3 Results-----------------------------------" << endl;
    cout << "Number of index nodes the process accesses: " << bptree.countLevels(bptree.root) << endl;

    // Retrieval process
    auto t3 = high_resolution_clock::now();

    BPlusTreeNode* target_node;
    bool is_duplicate;

    tie(target_node, is_duplicate) = bptree.search_key(500);

    std::set<int>::iterator it;
    std::pair<std::set<int>::iterator,bool> ret;
    std::set<int> myset;
    uchar* block_address;
    int block_id, offset;
    float num_records = 1.0;
    float sum_averageRating = 0.0;

    for (int i = 0; i < target_node -> numKeys; i++){
        if (target_node -> keys[i] == 500){
            for (int j = 0; j < target_node -> recordLists[i].size(); j++){
                Record* record_ptr = target_node->recordLists[i][j];
                //cout << "Record " << j + 1 << " >>> " << "tconst: " << record_ptr->tconst <<  ", NumVotes: " << record_ptr->numVotes << ", averageRating: " << record_ptr->averageRating << endl;
                tie(block_address, block_id, offset) = memory_disk.retrieve_block_from_record_ptr(record_ptr);
                ret = myset.insert(block_id);
                sum_averageRating += record_ptr->averageRating;
                num_records++;
            }
        }
    }

    int num_data_blocks_accessed = myset.size();
    float average_averageRating = (sum_averageRating / num_records);

    auto t4 = high_resolution_clock::now();

    cout << "Number of data blocks the process accesses: " << num_data_blocks_accessed << endl;
    cout.precision(2);
    cout << "Average of averageRating of records returned:  " << average_averageRating << endl;
    cout << "Running time of retrieval process: " << endl;
    auto ms_2 = duration_cast<milliseconds>(t4 - t3);
    //auto ms_double = duration_cast<milliseconds>(t4 - t3);
    duration<double, std::milli> ms_double = t4 - t3;

    std::cout << ms_double.count() << "ms\n";


    auto t5 = high_resolution_clock::now();
    int num_data_blocks = 0;

    for (int i = 0; i < memory_disk.blocks_in_use; i++){
        uint block_id, num_of_elements;
        vector <tuple<uchar*, int>> record_address;
        num_data_blocks++;
        tie(block_id, num_of_elements,record_address) = memory_disk.read_nth_data_block(i);
        for (int j = 0; j < num_of_elements; j++){
            Record* record_ptr = (Record*) (get<0>(record_address[j]) + get<1>(record_address[j]));
            if (record_ptr -> numVotes == 500){
                break; // record with numVotes == 500 found
            }
        }
    }

    auto t6 = high_resolution_clock::now();

    auto ms_3 = duration_cast<milliseconds>(t6 - t5);

    std::cout << "Number of data blocks accessed by brute-force linear scan: " << num_data_blocks << endl;
    std::cout << "Running time for brute-force linear scan: " << duration<double>(ms_3).count() << "ms\n";


    cout << "---------------------------------------------------------------------------------------------" << endl;

    cout << "------------------------------Experiment 4 Results-------------------------------------------" << endl;
    int num_index_nodes_accessed;
    vector<Record*> record_vector;

    auto t7 = high_resolution_clock::now();
    
    // Retrieval process
    tie(record_vector, num_index_nodes_accessed) = bptree.search_target_range(30000, 40000);

    num_records = 1.0;
    sum_averageRating = 0.0;
    myset.clear();

    //cout << "Keys of records retrieved: " << endl;

    for (int i = 0; i < record_vector.size(); i++){
        tie(block_address, block_id, offset) = memory_disk.retrieve_block_from_record_ptr(record_vector[i]);
        Record* record_ptr = record_vector[i];
        ret = myset.insert(block_id);
        //cout << record_ptr -> numVotes << endl;
        sum_averageRating += record_ptr->averageRating;
        num_records++;
    }

    auto t8 = high_resolution_clock::now();

    auto ms_4 = duration_cast<milliseconds>(t8 - t7);

    average_averageRating = (sum_averageRating/num_records);

    cout << "Number of index nodes the process accesses: " << num_index_nodes_accessed << endl;
    cout << "Number of data blocks the process accesses: " << myset.size() << endl;
    cout << "Average of averageRatings: " << average_averageRating << endl;
    cout << "Running time of retrieval process: " << duration<double>(ms_4).count() << "ms\n";

    auto t9 = high_resolution_clock::now();
    num_data_blocks = 0;

    for (int i = 0; i < memory_disk.blocks_in_use; i++){
        uint block_id, num_of_elements;
        vector <tuple<uchar*, int>> record_address;
        num_data_blocks++;
        tie(block_id, num_of_elements,record_address) = memory_disk.read_nth_data_block(i);
        for (int j = 0; j < num_of_elements; j++){
            Record* record_ptr = (Record*) (get<0>(record_address[j]) + get<1>(record_address[j]));
            if (record_ptr -> numVotes >= 30000 && record_ptr -> numVotes <= 40000){
                break; // record with numVotes == 500 found
            }
        }
    }

    auto t10 = high_resolution_clock::now();

    auto ms_5 = duration_cast<milliseconds>(t10 - t9);
    
    cout << "Number of data blocks accessed by brute-force linear scan: " << num_data_blocks << endl;
    cout << "Running time for brute-force linear scan: " << duration<double>(ms_5).count() << "ms\n";

    cout << "------------------------------Experiment 5 Results-------------------------------------------" << endl;
    auto t11 = high_resolution_clock::now();
    
    bptree.remove(1000);
    
    auto t12 = high_resolution_clock::now();

    auto ms_6 = duration_cast<milliseconds>(t12 - t11);

    cout << "Number of nodes of updated B+ tree: " << bptree.countNodes(bptree.root) << endl;
    cout << "Number of levels of updated B+ tree: " << bptree.countLevels(bptree.root) << endl;
    cout << "Content of root node of updated B+ tree: " << endl;
    bptree.print_root_node();
    cout << "Running time of removal process: " << duration<double>(ms_6).count() << " ms" << endl;

    auto t13 = high_resolution_clock::now();
    num_data_blocks = 0;

    for (int i = 0; i < memory_disk.blocks_in_use; i++){
        uint block_id, num_of_elements;
        vector <tuple<uchar*, int>> record_address;
        num_data_blocks++;
        tie(block_id, num_of_elements,record_address) = memory_disk.read_nth_data_block(i);
        for (int j = 0; j < num_of_elements; j++){
            Record* record_ptr = (Record*) (get<0>(record_address[j]) + get<1>(record_address[j]));
            if (record_ptr -> numVotes >= 30000 && record_ptr -> numVotes <= 40000){
                break; // record with numVotes == 500 found
            }
        }
    }

    auto t14 = high_resolution_clock::now();

    auto ms_7 = duration_cast<milliseconds>(t14 - t13);
    
    cout << "Number of data blocks accessed by brute-force linear scan: " << num_data_blocks << endl;
    cout << "Running time for brute-force linear scan: " << duration<double>(ms_7).count() << "ms\n";

    return 0;
}
    