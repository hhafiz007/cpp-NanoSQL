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
#include<algorithm>

#define DEBUG_MODE

const int HEADER_SIZE = 100;
const int pageHeader = 8;
bool printTables = false;
bool debugStage = false;
const int interiorTablePage = 5;

struct MyTuple {
    bool filter= false;
    std::string  key;
    std::string value;
};



std::string stripKey(const std::string& str) {
    // Find the position of the first non-zero character
    size_t firstNonSpace = str.find_first_not_of(' ');
   
  
    // Find the position of the last non-zero character
    size_t lastNonSpace = str.find_last_not_of(' ');

    // Extract the substring without leading and trailing zeroes
    return str.substr(firstNonSpace, lastNonSpace - firstNonSpace + 1);
}

std::string stripValue(const std::string& str) {
// Find the position of the first non-zero character
    size_t firstNonSpace = str.find_first_not_of(' ');
   
  
    // Find the position of the last non-zero character
    size_t lastNonSpace = str.find_last_not_of(' ');

    // Extract the substring without leading and trailing zeroes
     return str.substr(firstNonSpace+1, (lastNonSpace-1) - (firstNonSpace+1)+1);
}






int processVarInt(std::vector<char> &database_file ,unsigned long rowAddress){
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


 int processHeader(std::vector<char> &database_file ,unsigned long rowAddress, std::vector<int> &header ){

    int totalBytes = int(database_file[rowAddress]);

    //  if (debugStage){
    //  std::cout <<"totalBytes  " <<totalBytes<<" end " << std:: endl;;
    //     }
    
   

    
    unsigned long i = rowAddress+1;

    while (i < rowAddress+totalBytes) {
      
        unsigned long  prev = i;
        unsigned long next = processVarInt(database_file,prev);
        unsigned short result = 0;
        unsigned long j ;
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



int getRowData(std::vector<char> &database_file , unsigned long rowAddress,std::vector<std::vector<std::string>> &tableData){

    unsigned long next = rowAddress;
    std::vector<int> header; 
    
    next = processVarInt(database_file,rowAddress);

    unsigned long prev = next;

    next = processVarInt(database_file,next);

    unsigned short result = 0;
    unsigned long j ;
    for (  j = 0 ; j +prev < next-1; j++) {
        result <<= 7;
        unsigned short currByte = static_cast<unsigned char>(database_file[prev+j]) ;
        // std :: cout << currByte << " curr byte "<<int(database_file[prev+j])<<std:: endl;
        result |=  (currByte) & 0b01111111;
        
    }
    result <<= 7;
    unsigned short currByte = static_cast<unsigned char>(database_file[prev+j]) ;
    result |= currByte;



    //  if (debugStage){
    //  std::cout <<"header start  " <<next<<" end " << std:: endl;
    //     }
    
    next = processHeader(database_file,next,header);
    int index = 0;
    
    std::vector<std::string> currRow;
    //     if (debugStage){
    //  std::cout <<"header size  " <<header.size()<<" end " << std:: endl;
    //     }
    for (int element : header) {
       
        if (element >= 13) {
            element = std::ceil((element -13)/2);
        }
   
    
        unsigned long startByte = next;
        unsigned long  endExclusive = next + element;
        // if (debugStage) {
        // std::cout <<"printing start element " <<startByte<<" end " << endExclusive<< std:: endl;
        // }
        std:: string currHeader;
        while (startByte < endExclusive) {
            currHeader = currHeader + database_file[startByte];
            startByte+=1;
        }

         if (index == 0) {
            // std::cerr << "cur row is " <<result<<"  "<<std::endl;
             currRow.push_back(std::to_string(result));
        }
        else{
             currRow.push_back(currHeader);
            
        }

       

        if (index == 1 && printTables == true) {
            std::cout <<currHeader<<"  "<<std::endl;
        }
        
       
         
        next = endExclusive;
        index+=1;
    }

    // std::cerr <<" total colucolsmns are are" << currRow.size() << std::endl;

   
    tableData.push_back(currRow);




    return next;

    








}


int  getLeafPage(std::vector<char> &database_file, unsigned long  start,std::vector<unsigned long> &leafAddresses){

    unsigned short pageType = static_cast<unsigned char>(database_file[start]);
    unsigned long pageStart = start;

    // std::cerr << " debug: The interior page start is" << start << "  "<<std::endl;
   

    
    if (pageType == interiorTablePage) {
        unsigned short num_table = (static_cast<unsigned char>(database_file[pageStart+4]) | (static_cast<unsigned char>(database_file[pageStart+3]) << 8));
        start +=12;
        int i =0;
        while (i < int(num_table)) {

    
                int startIndex = (2*i)+start; 

                unsigned short byte1 = static_cast<unsigned char>(database_file[startIndex]);
                unsigned short byte2 = static_cast<unsigned char>(database_file[startIndex+1]);
                unsigned short page_address =   ((byte1 << 8) | (byte2))+pageStart;
               

                unsigned long result1 = static_cast<unsigned char>(database_file[page_address]);
                unsigned long  result2 = static_cast<unsigned char>(database_file[page_address+1]);
                unsigned long  result3 = static_cast<unsigned char>(database_file[page_address+2]);
                unsigned long  result4 = static_cast<unsigned char>(database_file[page_address+3]);
               
                unsigned long  result =   ((result1 << 24) | (result2 << 16) | (result3 << 8) | (result4));
                //   std::cerr << " debug: The next   page result is" << result << "  "<<std::endl;


                int nextAdress = (result-1)*4096;

            
                // std::cerr << " debug: The next  interior page start is" << nextAdress << "  "<<std::endl;
                int j = getLeafPage(database_file,nextAdress,leafAddresses);
                i+=1;

                // std::cerr << " total interior page  is" << num_table << " curr page is  "<<i<<" "<<num_table<<" "<<std::endl;
        }
    }
    else
    {
        //  std::cerr << " debug: pputting page type is" << start << "  "<<std::endl;
leafAddresses.push_back(start);

    }



    
    return 0;
}




void printTableLeafPage(std::vector<char> &database_file , unsigned short num_table,unsigned long start,std::vector<std::vector<std::string>> &tableData) {
    
    

    // if (debugStage){
    //     std :: cerr << "the start d is"<<start<<"  ro ws "<< num_table << std::endl; 

    // }

    int pageStart = start-8;

    if (pageStart == 100 ){
        pageStart=0;
    } 

    unsigned short pageType = static_cast<unsigned char>(database_file[pageStart]);

    std::vector<unsigned long> leafAddresses;


    // std::cerr << " debug: The page type is" << pageType << "  "<<std::endl;

    if (int(pageType) == interiorTablePage) {
        debugStage = true;
        pageStart = getLeafPage(database_file,pageStart,leafAddresses);
        start = pageStart+8;
        // std::cerr<<"total leaf pages is "<< leafAddresses.size()<<"  ";
        
      

    }
    else{
        leafAddresses.push_back((pageStart));
    }


    for (unsigned long startAddress: leafAddresses){

        start = startAddress+8;
        pageStart = startAddress;
        if (startAddress == 0) {
            start = 108;
        
        }
         

        if (startAddress != 0) {

            unsigned short byte1 = static_cast<unsigned char>(database_file[startAddress+3]);
        unsigned short byte2 = static_cast<unsigned char>(database_file[startAddress+4]);
        num_table =   (byte1 << 8) | (byte2);


        }
        

    //   std::cerr<<"total  cell is "<< num_table<<"  "<<start<<std::endl;

        
    std::vector<unsigned long> cellAddress;

    // Calculate factorialass b
    for (int i = 0; i < num_table; i++) {
        int startIndex = (2*i)+start; 
        unsigned long byte1 = static_cast<unsigned char>(database_file[startIndex]);
        unsigned long byte2 = static_cast<unsigned char>(database_file[startIndex+1]);
        unsigned long page_address =   (byte1 << 8) | (byte2);
        cellAddress.push_back(pageStart+page_address);
        // std::cerr << "Logs from your program will appeaar here" << i <<"  "<<page_address+pageStart<<std::endl;
         if (printTables || debugStage ){
            // std::cout << "cell address  " <<pageStart+page_address<<std::endl; 
            // std:: cout << "index " << startIndex<<" value " << byte1<<std::endl;
            // std:: cout << "   2nd index " << startIndex+1<<" value " << byte2<<std::endl;
         }
        

    }

    // std::cerr<<"Page start is "<< startAddress<<"  celllls are "<< num_table <<" addresses are"<<\
    cellAddress.size()<<std::endl;

    //  if (tableData.size() >= 57) {
    //         break;
    //     }
        

    for (int i = 0; i < cellAddress.size(); ++i) {
        
    //  std::cerr << "processing row  " <<cellAddress[i]<<std::endl; 
        
       getRowData(database_file,cellAddress[i],tableData);
    //    if (tableData.size() >= 57) {
    //         break;
    //     }
    

       
    //    break;
        
    }

    // std::cerr << "the table size  is  " <<tableData.size()<<std::endl; 
   
    

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

std::vector<std::string> getRootPage(std::vector<std::vector<std::string> > &tableData,std:: string tableName,int &rootPage) {

    // std::cout << " tableName" << tableName;
std::string sqlStatement;

for (std::vector table : tableData){
      std::cerr << "Deebugtable Name: " << tableName <<"curr table is "<<table[1]<< std::endl;
    //  std::cerr <<" table name " <<tableName <<" page number" << table[3][0] << std::endl;
        if (table[1] == tableName){
            // std::cerr << "rootPage   " <<tableName <<  " number " <<int(table[3][0]) << std::endl;
            rootPage =  int(table[3][0]);
             sqlStatement = table[4];
        
           
            
        }

        // std :: cout<<" create table command is " << table[4] << std::endl;
       


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
    std::cerr << "contents  are"<<sqlStatement<<"  ";
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

std:: vector <std::string>  parseSelectColumns(std::string &query) {


    std:: vector <std::string> selectColumns;

    std::regex regex("SELECT\\s+(.*?)\\s+FROM", std::regex_constants::icase);
    std::smatch match;

    std::string content;

    if (std::regex_search(query, match, regex)) {
        content =  match[1].str();
         size_t found = content.find(',');

    // Check if the comma was found
        if (found != std::string::npos) {
            // std::cout << "The string contains a comma at position " << found << std::endl;
            // std::cout << content<<"  "<<content.size() <<std::endl;
            selectColumns = split(content, ',');
            for (std::string &column: selectColumns){
                if (column[0] == ' '){
                    column = column.substr(1,column.size()-1);

                }
            // std::cout <<"column name  "<< column << std :: endl;
            }
        }
        else{
            selectColumns.push_back(content);
        } 
        
    } 


    return selectColumns;




}





void printSelectColumns(   std::vector <std::vector<std::string>> &tableData, std::vector<std::string>  &columnNames, std::vector<std::string> selectColumn,MyTuple &filter)
{

    int m = columnNames.size();

    // if (filter.filter) {
    //     std::cout <<"filter is on  "<<filter.key << filter.value<<std::endl;
    // }



    
    int index = 0;

    std:: vector<int> columnIndices;

   

        for(std::string curColumn : selectColumn) {

             for (int i = 0 ; i < m ; i++){


            if (columnNames[i] ==  curColumn){

                columnIndices.push_back(i);
            }
            if (filter.filter && columnNames[i] == filter.key)
            {
                index = i;
            }
        }
        
        
    }

    // if (filter.filter) {
    //     std::cout <<"filterr is offff d ds"<<filter.key << filter.value<<std::endl;
    // }

//     for (int columnNamecur : columnIndices) {
//         if (columnIndices.size() == 1){
//             break;
//         }}
//         std :: cout << columnNamecur <<" printed column number" << std:: endl;
//     }
// // kk
    for (int j = 0 ; j < tableData.size();j++){

        // std :: cout << columnNames[index] << std :: endl;
        
        std :: string rowData;
        // if (filter.filter) {
        //     std::cout << "we should be here" <<tableData[j][index];
        // }

        std::string filterIndex = tableData[j][index] ;
         std::transform(filterIndex.begin(), filterIndex.end(), filterIndex.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
        if (filter.filter && filterIndex != filter.value){
            // std::cerr <<" It did not get select because filter is "<< filter.key << " table size is "<< tableData.size()<<std::endl;
            //  std::cerr <<" It did not get select because table value  is "<< filterIndex<<std::endl;
            //   std::cerr <<" It did not get select because table j value  is "<< j<<" "<<index<<" "<< tableData[j][1]<<std::endl;
            continue;
        }

        std::cerr<<"welcome for printing"<<std::endl;

        rowData = tableData[j][columnIndices[0]];
        int k =1;

        for (k = 1; k < columnIndices.size();k+=1)
        {
            rowData += '|'+tableData[j][columnIndices[k]];
        }

        std :: cout << rowData << std ::endl;


        

        // std :: cout << j<<std:: endl;


    }





}

MyTuple parseWhereFilter(std::string &query){

    // if (debugStage){

    // std::cout << "Welcome to filtering "<<std::endl;
    // }
   

    // Convert each character in the string to lowercase
    std::transform(query.begin(), query.end(), query.begin(), [](unsigned char c) {
        return std::tolower(c);
    });


    size_t found = query.find("where");
    std::cerr << "Debug: " << query << std::endl;
    MyTuple ans;
    if (found != std::string::npos) {
        // std::vector<std::string> tokens = split(query, "WHERE");
        std::string whereQuery = query.substr(found + 5);
        std::vector<std::string> whereTokens = split(whereQuery, '=');
        std::string key = whereTokens[0];
        std::string value = whereTokens[1];
    
        ans.key = stripKey(key);
        ans.value =stripValue(value);
        ans.filter = true;

        // std::cout << " wheree found "<<std::endl;
       






    }
    else {
        ans.filter = false;
        
    }

  

    return ans;






}

std:: string parseTableName (std::string &query){

        std::transform(query.begin(), query.end(), query.begin(), [](unsigned char c) {
        return std::tolower(c);
    });


  size_t fromPos = query.find("from");
    if (fromPos != std::string::npos) {
        // Find the start of the table name
        // std::cerr << " from "
        size_t tableNameStart = fromPos + 5; // "FROM" + space
        // Find the end of the table name
        size_t tableNameEnd = query.find_first_of(" \n\r\t,", tableNameStart);
        if (tableNameEnd != std::string::npos) {
            // Extract the table name
            return query.substr(tableNameStart, tableNameEnd - tableNameStart);
        }
        else{
            tableNameEnd = (query.size());
            return query.substr(tableNameStart, tableNameEnd - tableNameStart);
        }
    }
    return "";
    
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
    
    std::vector <std::vector<std::string> > tableData; 
    printTables  = true;
    printTableLeafPage(bytes,num_table,start,tableData);



    }
    else if (command.substr(0,15) == "select count(*)")  {
        // std :: cout << command << " great work  "<<std::endl;
        int start=108;
        std::vector <std::vector<std::string> > tableData; 

        
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
        std::cerr << "Debug2000: " << command << std::endl;
        std::vector <std::vector<std::string> > schemaData; 

        // printTables = true;

        printTableLeafPage(bytes,num_table,start,schemaData);
             
        std::cerr << "Debug3000: " << command << std::endl;

        std :: string tableName = parseTableName(command);
        
      
        
        int rootPage;
        std::vector<std::string> columnNames = getRootPage(schemaData,tableName,rootPage);

        for (std::string col : columnNames) {
            std::cerr << "col name is " << col<<std::endl;
        }

         std::cerr << "Debug4000: " << rootPage <<"   "<<tableName <<std::endl;
        
        start = (rootPage-1)*4096;
        // std :: cerr <<rootPage << "back here  " << int(bytes[start]) << "     "<<page_size <<" ";

        unsigned short cellCount=(static_cast<unsigned char>(bytes[start+4]) | (static_cast<unsigned char>(bytes[start+3]) << 8));
        

        // ,,, std::vector<std::string> columnNames = getRootPage(schemaData,tokens[queryLength-1],rootPage);

        
        std::cerr << "Deebug1: " << command << std::endl;
        
        
        std:: vector <std::string> selectColumns = parseSelectColumns(command);

        

        MyTuple selectFilter = parseWhereFilter(command);


        
        std::vector <std::vector<std::string> > tableData; 

        // int newAddress = ((rootPage-1)*4096)+8;
        // std :: cout <<rootPage << "back here  " << newAddress  <<" ";

        int newAddress = start+8;
        if (selectFilter.filter) {
            debugStage = true;
            // std::cout << " filter value=" <<selectFilter.value<< std::endl;
        }

      
        
        printTableLeafPage(bytes,cellCount,newAddress,tableData);

        std :: cerr << "back here  " << tableData[0].size()  <<" "<<tableData.size()<<std::endl; // change comments kk mm mm ss nn nn


         int index = 0;
        for (std::vector row : tableData){

           

            for (std::string value : row) {

                if (value == "Matris Ater Clementia (New Earth)")
                    {

                    
                std::cerr << "index is "<<index<< " value "<<value<<"  "; 
                 std::cerr <<std::endl;
                    }
            }
            index+=1;

           
            // break;



        }    


        printSelectColumns(tableData, columnNames , selectColumns,selectFilter);










    }
    //// std:: cout << command.substr(0,15);
    
    
    return 0;
}



