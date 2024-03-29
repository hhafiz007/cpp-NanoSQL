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
#include <iostream>
#include <iomanip>
#include <map>
#include <set>



using namespace std;


set<string> mySet;
map<std::string, int> myMap;

const int interiorIndexPage = 2;

unsigned long processVarIntIndex(std::vector<char> &database_file ,unsigned long rowAddress){
    // std::cout << "row Address: " << rowAddress << std::endl;

    int i = 0;
    bool isMSBSet = (static_cast<unsigned char>(database_file[rowAddress+i]) >> 7) & 1;
    //  std::cout << "msb bit  " << isMSBSet<<std::endl;

    while (isMSBSet) {
        // std::cout << "setting bit" << std::endl;
        i += 1;
        isMSBSet = (static_cast<unsigned char>(database_file[rowAddress+i]) >> 7) & 1;
    }

    /// /std::cout << "printing payload bytes: " << static_cast<int>(database_file[rowAddress+i]) << std::endl;

    return rowAddress + i + 1;
}


unsigned long processHeaderIndex(std::vector<char> &database_file ,unsigned long rowAddress, std::vector<int> &header ){

    int totalBytes = int(database_file[rowAddress]);

    //  if (debugStage){
    //  std::cout <<"totalBytes  " <<totalBytes<<" end " << std:: endl;;
    //     }
    
   

    
    unsigned long i = rowAddress+1;

    while (i < rowAddress+totalBytes) {
      
        unsigned long  prev = i;
        unsigned long next = processVarIntIndex(database_file,prev);
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



unsigned long processRowData(std::vector<char> &database_file , unsigned long rowAddress,std::vector<unsigned long> rowIds,string indexValue){
    unsigned long next = rowAddress;

    unsigned long rowStartByte = rowAddress;
    std::vector<int> header; 
    next = processVarIntIndex(database_file,rowAddress);
   
    next = processHeaderIndex(database_file,next,header);
    int index = 0;
    
    std::vector<std::string> currRow;
   
    for (int element : header) {
       
        if (element >= 13) {
            element = std::ceil((element -13)/2);
        }
   
    
        unsigned long startByte = next;
        unsigned long  endExclusive = next + element;
      

        std:: string currHeader;

             if (index == 1) {
                unsigned short result = 0;
                unsigned long j ;
                for (  j = startByte ; j < endExclusive; j++) {
                    result <<= 8;
                    unsigned short currByte = static_cast<unsigned char>(database_file[j]) ;
                    // std :: cout << currByte << " curr byte "<<int(database_file[prev+j])<<std:: endl;
                    result |=  (currByte);
                }
                myMap[currRow[0]] = result;
                // cout << "  rowID  " << result << endl;
             


        }else{
        while (startByte < endExclusive) {
            // unsigned short currByte = static_cast<unsigned char>(database_file[startByte]) ;
            // cout <<currByte  <<" ";
            currHeader = currHeader + database_file[startByte];
            startByte+=1;
        }
        }

        if (index == 0  && currHeader > indexValue)
        {

            std:: cout << " I am greater" <<"  " << currHeader<<"  "<<indexValue<<std::endl;
            std:: cout <<currHeader<<"  ";
            
            if (mySet.find(currHeader) == mySet.end())
            {
             
             mySet.insert(currHeader);
            }
             return 1;
        }
        else if (index == 0 ) {
            std:: cout << " I am 0" <<"  " << currHeader<<std::endl;
            
        }

   



        // std:: cout << " cur header from index" <<index<<"  " << currHeader<<std::endl;

       
        currRow.push_back(currHeader);
            
        next = endExclusive;
        index+=1;
    }

     while (rowStartByte < next) {
            unsigned short currByte = static_cast<unsigned char>(database_file[rowStartByte]) ;
            cout <<currByte  <<" ";
            // currHeader = currHeader + database_file[startByte];
            rowStartByte+=1;
        }
      cout<<" "<<endl;

 

    return next;

    








}



void parseInteriorIndexPages(std::vector<char> &database_file,unsigned long pageStart,vector<unsigned long> rowIds,string indexValue){


    unsigned short pageType = static_cast<unsigned char>(database_file[pageStart]);

    // cout << " welcome to first page" << pageType <<"  "<<pageStart<<endl;

    unsigned long startAddress = pageStart;

    bool flag = false;

        // cout << " welcome to interior page" << pageType <<"  "<<endl;
            
    unsigned short byte1 = static_cast<unsigned char>(database_file[startAddress+3]);
    unsigned short byte2 = static_cast<unsigned char>(database_file[startAddress+4]);
    unsigned short num_table =   (byte1 << 8) | (byte2);

    unsigned long rightPointer;
    unsigned long leftPointer;

    if(pageType != 10) {

                unsigned long result1 = static_cast<unsigned char>(database_file[startAddress+8]);
                unsigned long  result2 = static_cast<unsigned char>(database_file[startAddress+9]);
                unsigned long  result3 = static_cast<unsigned char>(database_file[startAddress+10]);
                unsigned long  result4 = static_cast<unsigned char>(database_file[startAddress+11]);

                
                
                rightPointer =   (((result1 << 24) | (result2 << 16) | (result3 << 8) | (result4)))*4096;
    }




    unsigned long start = startAddress+12;

    if (pageType == 10){
        start-=4;
    }

    std::vector<unsigned long> cellAddress;
        for (int i = 0; i < num_table; i++) {
                                                int startIndex = (2*i)+start; 
                                                unsigned long byte1 = static_cast<unsigned char>(database_file[startIndex]);
                                                unsigned long byte2 = static_cast<unsigned char>(database_file[startIndex+1]);
                                                unsigned long page_address =   (byte1 << 8) | (byte2);
                                                cellAddress.push_back(pageStart+page_address);
        }

            //  cout << "total cells " <<cellAddress.size()<<endl; 

             

             for (int i = 0; i < cellAddress.size(); ++i) {

                leftPointer = 0;


                if(pageType != 10) {

                unsigned long result1 = static_cast<unsigned char>(database_file[cellAddress[i]]);
                unsigned long  result2 = static_cast<unsigned char>(database_file[cellAddress[i]+1]);
                unsigned long  result3 = static_cast<unsigned char>(database_file[cellAddress[i]+2]);
                unsigned long  result4 = static_cast<unsigned char>(database_file[cellAddress[i]+3]);

                
               
                leftPointer =   (((result1 << 24) | (result2 << 16) | (result3 << 8) | (result4)))*4096;
                }

                unsigned long childAddress = cellAddress[i]+4;
                if (pageType == 10){

                            childAddress=cellAddress[i];
                // cout << "  Processing child " << i<<"   "<<childAddress << endl;
                         }

                unsigned long next = processRowData(database_file,childAddress,rowIds,indexValue);

                if ( pageType == 2 && next == 1) {
                    cout << " welcome to next  address" << leftPointer <<"  ";
                    flag = true;
                    parseInteriorIndexPages(database_file,leftPointer,rowIds,indexValue);
                    break;
                }
                // else if (rowIds.size()){
                //      break;

                // }

       
        }

    
        
            

            







}

void printIndexLeafPage(std::vector<char> &database_file , unsigned short rootPage,string indexValue) {
    
   

    
    std::vector<unsigned long> rowIds;



   
    unsigned long pageStart = 916840448;

    unsigned long pageType = static_cast<unsigned char>(database_file[916840448]);
    
      
    
    

    parseInteriorIndexPages(database_file,pageStart,rowIds,indexValue);

   std::cout << "Contents of the map:" << std::endl;
    for (const auto& pair : myMap) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
        


        //  bfor (unsigned long startAddress: leafAddresses){ jjj

        //     start = startAddress+8;
        //     pageStart = startAddress;
        //     if (startAddress == 0) {
        //         start = 108;
            
        //     }
            

        //     if (startAddress != 0) {

        //         unsigned short byte1 = static_cast<unsigned char>(database_file[startAddress+3]);
        //     unsigned short byte2 = static_cast<unsigned char>(database_file[startAddress+4]);
        //     num_table =   (byte1 << 8) | (byte2);


        //     }
            


        // std::vector<unsigned long> cellAddress;

        
        // for (int i = 0; i < num_table; i++) {
        //     int startIndex = (2*i)+start; 
        //     unsigned long byte1 = static_cast<unsigned char>(database_file[startIndex]);
        //     unsigned long byte2 = static_cast<unsigned char>(database_file[startIndex+1]);
        //     unsigned long page_address =   (byte1 << 8) | (byte2);
        //     cellAddress.push_back(pageStart+page_address);
        
            

        // } 

        // for (int i = 0; i < cellAddress.size(); ++i) {
            
    
        //    getRowData(database_file,cellAddress[i],tableData);
    
            
        // }

        

        // }
   
}



void printHello(){

    cout <<"hello from the other side"<<endl;
}