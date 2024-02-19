#ifndef GIT_STARTER_CPP_COMMAND_H
#define GIT_STARTER_CPP_COMMAND_H

#include <cstring>
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
void printIndexLeafPage(std::vector<char> &database_file , unsigned short rootPage,string indexValue);

#endif // GIT_STARTER_CPP_COMMAND_H
