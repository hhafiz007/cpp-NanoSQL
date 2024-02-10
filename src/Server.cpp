#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>

const int HEADER_SIZE = 100;
const int pageHeader = 8;

struct schemaRow {
    // Member variables
    int type;
    std::string name;
    std::string tblName;
    std::string rootPage;
    std::string sql;
};

int main(int argc, char* argv[]) {
    // You can use print statements as follows for debugging, they'll be visible when running tests.
    std::cout << "Logs from your program will appear here" << std::endl;

    if (argc != 3) {
        std::cerr << "Expected two arguments" << std::endl;
        return 1;
    }

    std::string database_file_path = argv[1];
    std::string command = argv[2];

    if (command == ".dbinfo") {
        std::ifstream database_file(database_file_path, std::ios::binary);
        if (!database_file) {
            std::cerr << "Failed to open the database file" << std::endl;
            return 1;
        }

        // Uncomment this to pass the first stage
        database_file.seekg(16);  // Skip the first 16 bytes of the header
        char buffer[2];
        database_file.read(buffer, 2);
        unsigned short page_size = (static_cast<unsigned char>(buffer[1]) | (static_cast<unsigned char>(buffer[0]) << 8));
        database_file.seekg(HEADER_SIZE+3);
        database_file.read(buffer, 2);
        unsigned short num_table = (static_cast<unsigned char>(buffer[1]) | (static_cast<unsigned char>(buffer[0]) << 8));
        std::cout << "database page size: " << page_size << std::endl;
        std::cout << "number of tables: " << num_table << std::endl;
        printTables(database_file,num_table);

        // unsigned short num_table = (static_cast<unsigned char>(buffer[1]) | (static_cast<unsigned char>(buffer[0]) << 8))
    
 

        
    }
    
    return 0;
}



void printTables(std::ifstream database_file , unsigned short num_table) {
    database_file.seekg(108); 
    int totalBytes = 2*num_table;
    char buffer[totalBytes];
    database_file.read(buffer, totalBytes);
    std::vector<unsigned short> cellAddress;

    // Calculate factorial
    for (int i = 0; i < num_table; i++) {
        int startIndex = 2*i; 
        unsigned short page_address = (static_cast<unsigned char>(buffer[startIndex+1]) | (static_cast<unsigned char>(buffer[startIndex]) << 8));
        cellAddress.push_back(page_address);
    }
    std::cout << "adresses of " << cellAddress << std::endl;
}
