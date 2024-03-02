# include <iostream>
# include <cmath>
# include <malloc.h>
# include <tuple>
# include <fstream>
# include <sstream>
# include <string>
# include <vector>
# include "disk.h" //user-defined header file

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

    for (const auto& record : records) {
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
    
    return 0;
}
    