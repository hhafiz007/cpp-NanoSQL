#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <variant>
#include <charconv>
#include <cmath>
#include <regex>
#include <sstream>
#include <string>


const int HEADER_SIZE = 100;
const int pageHeader = 8;
bool printTables = false;

struct schemaRow {
    // Member variables
    int type;
    std::string name;
    std::string tblName;
    std::string rootPage;
    std::string sql;
};

unsigned int littleEndianToInt(const unsigned char* bytes) {
    unsigned int result;
    std::from_chars((const char*)bytes, (const char*)bytes + sizeof(unsigned int), result);
    return result;
}





int processVarInt(std::vector<char> &database_file ,unsigned short rowAddress){
    // std::cout << "row Address: " << rowAddress << std::endl;

    int i = 0;
    bool isMSBSet = (static_cast<unsigned char>(database_file[rowAddress+i]) >> 7) & 1;
    //  std::cout << "msb bit  " << isMSBSet<<std::endl;

    while (isMSBSet) {
        // std::cout << "setting bit" << std::endl;
        i += 1;
        isMSBSet = (static_cast<unsigned char>(database_file[rowAddress+i]) >> 7) & 1;
    }

    // std::cout << "printing payload bytes: " << static_cast<int>(database_file[rowAddress+i]) << std::endl;

    return rowAddress + i + 1;
}


 int processHeader(std::vector<char> &database_file ,unsigned short rowAddress, std::vector<int> &header ){

    int totalBytes = int(database_file[rowAddress]);
   

    
    int i = rowAddress+1;

    while (i < rowAddress+totalBytes) {
      
        int prev = i;
        int next = processVarInt(database_file,prev);
        unsigned short result = 0;
        int j ;
        for (  j = 0 ; j +prev < next-1; j++) {
            result <<= 8;
            unsigned short currByte = static_cast<unsigned char>(database_file[prev+j]) ;
            // std :: cout << currByte << " curr byte "<<int(database_file[prev+j])<<std:: endl;
            result |=  (currByte) & 0b01111111;
            
        }
        result <<= 8;
        unsigned short currByte = static_cast<unsigned char>(database_file[prev+j]) ;
        result |= currByte;
        header.push_back(result);
        // std::cout << " indices" << prev <<"   "<< next<<" "<<rowAddress+totalBytes << "   " << result <<std:: endl;
        i = next;
       
        // break;
        

    }

    
    





    return i;
 }



int getRowData(std::vector<char> &database_file , unsigned short rowAddress,std::vector<std::vector<std::string>> &tableData){

    int next = rowAddress;
    std::vector<int> header; 
    
    next = processVarInt(database_file,rowAddress);
    next = processVarInt(database_file,next);
    
    next = processHeader(database_file,next,header);
    int index = 0;
    
    std::vector<std::string> currRow;

    for (int element : header) {
       
        if (element >= 13) {
            element = std::ceil((element -13)/2);
        }
   
    
        int startByte = next;
        int endExclusive = next + element;
        // std::cout <<"printing start element " <<startByte<<" end " << endExclusive<< std:: endl;
        std:: string currHeader;
        while (startByte < endExclusive) {
            currHeader = currHeader + database_file[startByte];
            startByte+=1;
        }

        currRow.push_back(currHeader);

        if (index == 1 && printTables == true) {
            std::cout <<currHeader<<" ";
        }
        
        next = endExclusive;
        index+=1;
    }

    tableData.push_back(currRow);




    return next;

    








}


void printTableLeafPage(std::vector<char> &database_file , unsigned short num_table,int start,std::vector<std::vector<std::string>> &tableData) {
    
    std::vector<unsigned short> cellAddress;

    if (printTables){
        std :: cout << "the start is"<<start << std::endl; 

    }

    // Calculate factorialass b
    for (int i = 0; i < num_table; i++) {
        int startIndex = 2*i+start; 
        unsigned short page_address = (static_cast<unsigned char>(database_file[startIndex+1]) | (static_cast<unsigned char>(database_file[startIndex]) << 8));
        cellAddress.push_back(page_address);
        // std::cout << "Logs from your program will appeaar here" << page_address<<std::endl;
        

    }

    for (int i = 0; i < cellAddress.size(); ++i) {
       getRowData(database_file,cellAddress[i],tableData);
    //    break;
        
    }


   
}

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::vector<std::string> getRootPage(std::vector<std::vector<std::string>> &tableData,std:: string tableName,int &rootPage) {

    // std::cout << " tableName" << tableName;
std::string sqlStatement;

for (std::vector table : tableData){
        if (table[1] == tableName){
            // std::cout << "rootPage   " <<tableName <<  " number " <<int(table[3][0]) << std::endl;
            rootPage =  int(table[3][0]);
            
        }

        // std :: cout<<" create table command is " << table[4] << std::endl;
        sqlStatement = table[4];
        


}

std::vector<std::string> columnNames;

        std:: string contents;
     std::regex regex("\\(([^()]*)\\)"); // Pattern to match content within parentheses
    std::sregex_iterator iter(sqlStatement.begin(), sqlStatement.end(), regex);
    std::sregex_iterator end;

    for (; iter != end; ++iter) {
        contents += (*iter)[0].str().substr(1, (*iter)[0].str().size() - 2); // Remove surrounding parentheses
    }
    std::vector<std::string> tokens = split(contents, ',');
    // std::cout << "contents  " << contents;
    int index = 0;

    while( index <tokens.size()){
        // std::cout << "  token s " << tokens[index] << std::endl;
        std::vector<std::string> columnTokens = split(tokens[index], ' ');
       
        if (index ==  0 or columnTokens[0].size() >= 1){
            columnNames.push_back(columnTokens[0]);
            //  std::cout << "The token size is  " << columnTokens[0];
        }
        else{
                 columnNames.push_back(columnTokens[1]);
                //   std::cout << "The token size is  " << columnTokens[1];

        }
   
        index+=1;


    }

    

   

    return columnNames;

}

std::string parseSelectColumns(std::string &query) {


    std:: vector <std::string> selectColumns;

    std::regex regex("SELECT\\s+(.*?)\\s+FROM", std::regex_constants::icase);
    std::smatch match;

    std::string content;

    if (std::regex_search(query, match, regex)) {
        content =  match[1].str();
        // std::cout << content<<content.size() <<std::endl;
    } 


    return content;




}


void printSelectColumns(   std::vector <std::vector<std::string>> &tableData, std::vector<std::string>  &columnNames, std::string selectColumn)
{

    int m = columnNames.size();

    int columnBool[m];
    int index = 0;

    for (int i = 0 ; i < m ; i++){
        columnBool[i] = 0;
        // std::cout << columnNames[i]<<std::endl;
        if (columnNames[i] == selectColumn){
            // std :: cout << "tesstting column" << columnNames[i] <<" " << tableData.size();
            index = i-1;
            // std :: cout << "tableData is " << tableData.size() <<" "<<std::endl;
            columnBool[index] = 1;
        }
    }
// kk
    
    for (int j = 0 ; j < tableData.size();j++){

        
        std :: cout << "hello from table data "<< tableData[j].size()<<std::endl;
        

        // std :: cout << j<<std:: endl;


    }





}


int main(int argc, char* argv[]) {
    // You can use print statement;;s as follows for debugging, they'll be visible when running tests.
    // std::cout << "Logs from your program will appear here" << std::endl;

    if (argc != 3) {
        std::cerr << "Expected two arguments" << std::endl;
        return 1;
    }

    std::string database_file_path = argv[1];
    std::string command = argv[2];

    std::ifstream database_file(database_file_path, std::ios::binary);
        if (!database_file) {
            std::cerr << "Failed to open the database file" << std::endl;
            return 1;
        }

        
       
    // Get the file size
    database_file.seekg(0, std::ios::end);
    std::streampos fileSize = database_file.tellg();
    database_file.seekg(0, std::ios::beg);

    // Read the file into a vector of bytes
    std::vector<char> bytes(fileSize);
    database_file.read(bytes.data(), fileSize);
     database_file.seekg(16);  // Skip the first 16 bytes of the header
    char buffer[2];
    database_file.read(buffer, 2);
    unsigned short page_size = (static_cast<unsigned char>(buffer[1]) | (static_cast<unsigned char>(buffer[0]) << 8));
    database_file.seekg(HEADER_SIZE+3);
    database_file.read(buffer, 2);
    unsigned short num_table = (static_cast<unsigned char>(buffer[1]) | (static_cast<unsigned char>(buffer[0]) << 8));

    // Close the file
    database_file.close();
    // unsigned short num_table;

    if (command == ".dbinfo") {

        // Uncomment this to pass the first stage
       
        std::cout << "database page size: " << page_size << std::endl;
        std::cout << "number of tables: " << num_table << std::endl;
        

    // Output the number of bytes read herea
    
    

    

    

    }
    else if ( command == ".tables")
    {
        

    int start = 108;
    
    std::vector <std::vector<std::string>> tableData; 
    printTables  = true;
    printTableLeafPage(bytes,num_table,start,tableData);



    }
    else if (command.substr(0,15) == "select count(*)")  {
        // std :: cout << command << " great work  "<<std::endl;
        int start=108;
        std::vector <std::vector<std::string>> tableData; 

        
        printTableLeafPage(bytes,num_table,start,tableData);
        
        std::vector<std::string> tokens = split(command, ' ');
        
        int queryLength = tokens.size();
        int rootPage;
        
        std::vector<std::string> columnNames = getRootPage(tableData,tokens[queryLength-1],rootPage);
        start = (rootPage-1)*4096;
        unsigned short cellCount=(static_cast<unsigned char>(bytes[start+4]) | (static_cast<unsigned char>(bytes[start+3]) << 8));
        std::cout <<cellCount << std::endl;



    }
    else {
        int start=108;
        std::vector <std::vector<std::string>> schemaData; 

        printTables = true;

        printTableLeafPage(bytes,num_table,start,schemaData);
              std::vector<std::string> tokens = split(command, ' ');
        
        int queryLength = tokens.size();
        
      
        
        int rootPage;
        std::vector<std::string> columnNames = getRootPage(schemaData,tokens[queryLength-1],rootPage);
        
        start = (rootPage-1)*4096;
        // std :: cout <<rootPage << "back here  " << start  <<" ";

        unsigned short cellCount=(static_cast<unsigned char>(bytes[start+4]) | (static_cast<unsigned char>(bytes[start+3]) << 8));
        

        // std::vector<std::string> columnNames = getRootPage(schemaData,tokens[queryLength-1],rootPage);

        
        
        std:: string selectColumn = parseSelectColumns(command);


        
        std::vector <std::vector<std::string>> tableData; 

        // int newAddress = ((rootPage-1)*4096)+8;
        // std :: cout <<rootPage << "back here  " << newAddress  <<" ";

        int newAddress = 4054;
      

        printTableLeafPage(bytes,cellCount,newAddress,tableData);

        // std :: cout << "back here  " << start  <<" ";

        printSelectColumns(tableData, columnNames , selectColumn);










    }
    // std:: cout << command.substr(0,15);
    
    
    return 0;
}



