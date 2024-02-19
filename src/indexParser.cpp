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
using namespace std;

const int interiorIndexPage = 2;

int processVarIntIndex(std::vector<char> &database_file ,unsigned long rowAddress){
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


 int processHeaderIndex(std::vector<char> &database_file ,unsigned long rowAddress, std::vector<int> &header ){

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



int processRowData(std::vector<char> &database_file , unsigned long rowAddress,vector<unsigned long> rowIds){
    unsigned long next = rowAddress;
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
        while (startByte < endExclusive) {
            currHeader = currHeader + database_file[startByte];
            startByte+=1;
        }

        std:: cerr << " cur header from index" << currHeader<<std::endl;

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



void parseInteriorIndexPages(std::vector<char> &database_file,unsigned long pageStart,vector<unsigned long> rowIds){


    unsigned short pageType = static_cast<unsigned char>(database_file[pageStart]);

    unsigned long startAddress = pageStart;
    if (int(pageType) == interiorIndexPage) {
            
            unsigned short byte1 = static_cast<unsigned char>(database_file[startAddress+3]);
            unsigned short byte2 = static_cast<unsigned char>(database_file[startAddress+4]);
            unsigned short num_table =   (byte1 << 8) | (byte2);
            start = startAddress+12;
            std::vector<unsigned long> cellAddress;
             for (int i = 0; i < num_table; i++) {
                int startIndex = (2*i)+start; 
                unsigned long byte1 = static_cast<unsigned char>(database_file[startIndex]);
                unsigned long byte2 = static_cast<unsigned char>(database_file[startIndex+1]);
                unsigned long page_address =   (byte1 << 8) | (byte2);
                cellAddress.push_back(pageStart+page_address);
             }

             for (int i = 0; i < cellAddress.size(); ++i) {


            

                unsigned long result1 = static_cast<unsigned char>(database_file[cellAddress[i]]);
                unsigned long  result2 = static_cast<unsigned char>(database_file[cellAddress[i]+1]);
                unsigned long  result3 = static_cast<unsigned char>(database_file[cellAddress[i]+2]);
                unsigned long  result4 = static_cast<unsigned char>(database_file[cellAddress[i]+3]);

                
               
                unsigned long  leftPointer =   ((result1 << 24) | (result2 << 16) | (result3 << 8) | (result4));

                next = processRowData(database_file,cellAddress[i]+4,rowIds)






    
            
        }
        

            

        }
    else {


    }







}

void printIndexLeafPage(std::vector<char> &database_file , unsigned short rootPage,string indexValue) {
    
   
    unsigned long pageStart = (rootPage)*4096;
   
    

    cout << " welcome to root page" << rootPage <<"  "<<indexValue<<"  "<<pageStart<<endl;
    std::vector<unsigned long> rowIds;




        


        // for (unsigned long startAddress: leafAddresses){

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