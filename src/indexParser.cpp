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

void printIndexLeafPage(std::vector<char> &database_file , unsigned short rootPage) {
    
   
    unsigned long pageStart = (rootPage-1)*4096
   
    unsigned short pageType = static_cast<unsigned char>(database_file[pageStart]);

    cout << " welcome to root page" << rootPage <<"  "<<pageType<<"  "<<pageStart<<endl;


    

        // std::vector<unsigned long> leafAddresses;

        // if (int(pageType) == interiorTablePage) {
        //     debugStage = true;
        //     pageStart = getLeafPage(database_file,pageStart,leafAddresses);
        //     start = pageStart+8;

        // }
        // else{
        //     leafAddresses.push_back((pageStart));
        // }


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