#include <cstring>
#include <iostream>
#include <fstream>

const int HEADER_SIZE = 100;
const int pageHeader = 8;

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
        database_file.seekg(HEADER_SIZE+pageHeader);
        char buff[4];
        database_file.read(buff, 4);
        // unsigned short num_table = (static_cast<unsigned char>(buffer[1]) | (static_cast<unsigned char>(buffer[0]) << 8));
        unsigned long cellAdd = (static_cast<unsigned char>(buffer[3]) |
                         (static_cast<unsigned char>(buffer[2]) << 8) |
                         (static_cast<unsigned char>(buffer[1]) << 16) |
                         (static_cast<unsigned char>(buffer[0]) << 24));
        std::cout << "address: " << cellAdd << std::endl;
        
        unsigned long* ptr = reinterpret_cast<unsigned long>(cellAdd);
        std::cout << "Value at the address: " << *ptr << std::endl;

        
    }
    
    return 0;
}
