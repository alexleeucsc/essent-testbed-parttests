#ifndef PARSEGRAPH_H
#define PARSEGRAPH_H


#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <hash_map>
#include <set>

#include <unordered_map>
#include <map>

#include <stdio.h>
#include <time.h>

#include <iostream>
#include <fstream>
#include "Graph.hpp"



std::vector<std::vector<int>> processIntToListDict(std::string inputString){
    std::vector<std::vector<int>> indexList;
    std::string currentToken;
    int currentKey = 0;
    int currentTokenMode = 0;
    int idx = 0;
    //count number of times <1> occurs to set vector lens
    int idxcount = 0;
    size_t pos = 0;
    while ((pos = inputString.find("<1>", pos)) != std::string::npos) {
        idxcount++;
        pos += 3;
    }
    indexList.resize(idxcount, std::vector<int>());
    for (idx; idx < inputString.size()-2; idx ++) {
        if(inputString.substr(idx,3) == "<2>"){
            ////std::cout << "key: " << currentToken << "\t";
            // std::vector<int> currentValue;
            // currentKey = std::stoi(currentToken);
            // indexList[currentKey] = currentValue;
            if(currentToken.size()!=0){
                indexList[currentKey].push_back(std::stoi(currentToken));
            }
            currentToken.clear();
            idx = idx + 2;
        }else if(inputString.substr(idx,3) == "<1>"){
            ////std::cout << "value_end: " << currentToken << "\t";
            if(currentToken.size()!=0){
                indexList[currentKey].push_back(std::stoi(currentToken));
            }
            currentKey++;
            currentToken.clear();
            idx = idx + 2;
        }else{
            currentToken.push_back(inputString[idx]);
        }
    }
    for (idx; idx < inputString.size(); idx ++) {
        currentToken.push_back(inputString[idx]);
    }
    ////std::cout << "value_end: " << currentToken << "\t";
    if(currentToken.size()!=0){
        indexList[currentKey].push_back(std::stoi(currentToken));
    }
    currentToken.clear();
    return indexList;
}

bool validString(std::string instring){
    for(int i=0; i<instring.size(); i++){
        if(!std::isdigit(instring[i])){
                return false;
        }
    }
    return true;
}

std::vector<int> processList(std::string inputString){
    std::vector<int> outList;
    std::string currentToken;
    int currentKey;
    int currentTokenMode = 0;
    int idx = 0;
    for (idx; idx < inputString.size()-2; idx ++) {
        if(inputString.substr(idx,3) == "<1>"){
            ////std::cout << "value: " << currentToken << "\t";
            if(!validString(currentToken)){
                //std::cout << "PROBLEM!\n";
                exit(100);
            }
            try{
                outList.push_back(std::stoi(currentToken));
            }catch (std::invalid_argument){
                //std::cout << "current token? " << currentToken << "\n";
            }
            currentToken.clear();
            idx = idx + 3;
        }
        currentToken.push_back(inputString[idx]);
    }
    for (idx; idx < inputString.size(); idx ++) {
        currentToken.push_back(inputString[idx]);
    }
    //std::cout << "value_end: " << currentToken << "\t";
            if(validString(currentToken)){
                //std::cout << "PROBLEM!\n";
                exit(100);
            }
    //outList.push_back(std::stoi(currentToken));
    currentToken.clear();
    idx = idx + 3;
    return outList;
}

graphInfo processFile(const std::string& fileName) {
    std::vector<std::vector<int>> bigList;
    std::ifstream inputFile(fileName);
    graphInfo graphObj;
    std::set<int> printStatementsClump;
    bool printStmtsRead = false;
    if (inputFile.is_open()) {
        std::string line;

        while (std::getline(inputFile, line)) {
            if (line == "InNeigh") {
                std::getline(inputFile, line);
                std::vector<std::vector<int>> tmp = processIntToListDict(line);
                graphObj.inNeigh.resize(tmp.size());
                graphObj.inNeigh = tmp;
            }
            else if (line == "OutNeigh") {
                std::getline(inputFile, line);
                std::vector<std::vector<int>> tmp = processIntToListDict(line);
                graphObj.outNeigh.resize(tmp.size());
                graphObj.outNeigh = tmp;
            }
            else if (line == "validNodes") {
                if(!printStmtsRead){
                    //std::cout << "need printStatementsClump first! these have to be filtered out\n\n";
                    exit(111);
                }
                std::getline(inputFile, line);
                std::vector<int> tmp = processList(line);
                graphObj.validNodes.resize(tmp.size());
                int validNodeIdx1 = 0;
                for (auto vNode : tmp){
                    if(printStatementsClump.find(vNode) == printStatementsClump.end()){
                        graphObj.validNodes[validNodeIdx1] = vNode;
                        validNodeIdx1++;
                    }
                }
            }
            else if (line == "printStatementsClump") {
                std::getline(inputFile, line);
                std::vector<int> printStatementsClumpVec = processList(line);
                for(auto pStmt : printStatementsClumpVec){
                    printStatementsClump.insert(pStmt);
                }
                printStmtsRead = true;
            }
        }
        inputFile.close();
    } else {
        //std::cout << "Unable to open file: " << fileName << std::endl;
    }
    return graphObj;
}




void outputMGNeighbors(graphInfo mg){
    std::ofstream myfile;
    myfile.open ("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart_2_topotest/example.txt");
    myfile << "InNeigh:";
    for(auto incomingNodes : mg.inNeigh){
      for(auto member : incomingNodes){
        myfile << member << "<2>";
      }
      myfile << "<1>";
    }
    myfile << "\n";
    myfile << "OutNeigh:";
    for(auto outgoingNodes : mg.outNeigh){
      for(auto member : outgoingNodes){
        myfile << member << "<2>";
      }
      myfile << "<1>";
    }
    myfile << "\n";
    myfile.flush();
    myfile.close();
}

void outputMGMergeInfo(graphInfo mg){
    std::ofstream myfile;
    myfile.open ("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart_2_topotest/example.txt", std::ios_base::app);
    myfile << "mergeIDToMembers:";
    for(auto mID_mem : mg.mergeIDToMembers){
        myfile << mID_mem.first << "<2>";
        for(auto member : mID_mem.second){
            myfile << member << "<3>";
        }
        myfile << "<1>";
    }
    myfile << "\n";
    myfile << "idToMergeID:";
    for(auto mergeID : mg.idToMergeID){
        myfile << mergeID << "<1>";
    }
    myfile << "\n";
    myfile.flush();
    myfile.close();
}




#endif