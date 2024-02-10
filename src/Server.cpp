#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <variant>

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




int processVarInt(std::vector<char> &database_file ,unsigned short rowAddress){
    // database_file.seekg(rowAddress);
     std::cout<<"row Address" << rowAddress  <<std::endl;

    // vector<int> payload;

    int i = 0;

    while ((int(database_file[i]) >> 7) & 1) {
        i+=1;
    }

    std::cout<<"printing payload bytes" << int(database_file[rowAddress+i])<<" " <<int(database_file[rowAddress+i+1])   <<std::endl;

    return rowAddress+i+1;

 }



int getRowData(std::vector<char> &database_file , unsigned short rowAddress){

    int next = rowAddress;
    std::vector<int> header; 
    
    next = processVarInt(database_file,rowAddress);
    next = processVarInt(database_file,next);
    next = processVarInt(database_file,next);
    next = processHeader()


    return next;

    








}


void printTables(std::vector<char> &database_file , unsigned short num_table,int start) {
    
    std::vector<unsigned short> cellAddress;

    // Calculate factorialass b
    for (int i = 0; i < num_table; i++) {
        int startIndex = 2*i+start; 
        unsigned short page_address = (static_cast<unsigned char>(database_file[startIndex+1]) | (static_cast<unsigned char>(database_file[startIndex]) << 8));
        cellAddress.push_back(page_address);
         std::cout << "Logs from your program will appear here" << page_address<<std::endl;
        

    }

    for (int i = 0; i < cellAddress.size(); ++i) {
       getRowData(database_file,cellAddress[i]);
       break;
        
    }


   
}


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
       
    // Get the file size
    database_file.seekg(0, std::ios::end);
    std::streampos fileSize = database_file.tellg();
    database_file.seekg(0, std::ios::beg);

    // Read the file into a vector of bytes
    std::vector<char> bytes(fileSize);
    database_file.read(bytes.data(), fileSize);

    // Close the file
    database_file.close();

    // Output the number of bytes read herea
    std::cout << "Number of bytes read: " << bytes.size() << std::endl;

    int start = 108;
    

    printTables(bytes,num_table,start);

    

    }
    
    
    return 0;
}



