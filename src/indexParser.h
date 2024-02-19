#ifndef GIT_STARTER_CPP_COMMAND_H
#define GIT_STARTER_CPP_COMMAND_H

#include <cstring>
#include <string>
#include <string>
#include <vector>

// Constants for command types
const int COUNT_COMMAND = 1;
const int COLUMNS_COMMAND = 2;

// Constants for column types
// enum ColumnType {
//     COLUMN_TYPE_TEXT = 1,
//     COLUMN_TYPE_INTEGER = 2
// };

// Function declaration for printHello
void printHello();
void printIndexLeafPage(std::vector<char> &database_file , unsigned short rootPage,std::string indexValue);
void parseInteriorIndexPages(std::vector<char> &database_file,unsigned long pageStart,std::vector<unsigned long> rowIds);
int processRowData(std::vector<char> &database_file , unsigned long rowAddress,std::vector<unsigned long> rowIds);
int processHeaderIndex(std::vector<char> &database_file ,unsigned long rowAddress, std::vector<int> &header );
int processVarIntIndex(std::vector<char> &database_file ,unsigned long rowAddress);

#endif // GIT_STARTER_CPP_COMMAND_H
