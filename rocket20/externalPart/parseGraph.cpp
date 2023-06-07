#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <hash_map>

#include <unordered_map>
#include <map>

#include <stdio.h>
#include <time.h>

class graphInfo{
    public:
    std::map<int, std::vector<int>> inNeigh;
    std::map<int, std::vector<int>> outNeigh;
    std::vector<int> validNodes;
};

std::map<int, std::vector<int>> processIntToListDict(std::string inputString){
    std::map<int, std::vector<int>> hashmap;
    std::string currentToken;
    int currentKey;
    int currentTokenMode = 0;
    int idx = 0;
    for (idx; idx < inputString.size()-2; idx ++) {
        if(inputString.substr(idx,3) == "<2>"){
            std::cout << "key: " << currentToken << "\t";
            std::vector<int> currentValue;
            currentKey = std::stoi(currentToken);
            hashmap[currentKey] = currentValue;
            currentToken.clear();
            idx = idx + 3;
        }else if(inputString.substr(idx,3) == "<3>"){
            std::cout << "value: " << currentToken << "\t";
            if(currentToken.size() == 0){
                std::cout << "empty string bug\n";
                exit(100);
            }
            hashmap[currentKey].push_back(std::stoi(currentToken));
            currentToken.clear();
            idx = idx + 3;
        }else if(inputString.substr(idx,3) == "<1>"){
            std::cout << "value_end: " << currentToken << "\t";
            hashmap[currentKey].push_back(std::stoi(currentToken));
            currentToken.clear();
            idx = idx + 3;
        }
        currentToken.push_back(inputString[idx]);
    }
    for (idx; idx < inputString.size(); idx ++) {
        currentToken.push_back(inputString[idx]);
    }
    std::cout << "value_end: " << currentToken << "\t";
    hashmap[currentKey].push_back(std::stoi(currentToken));
    currentToken.clear();
    return hashmap;
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
            std::cout << "value: " << currentToken << "\t";
            std::cout << "   ";
            std::cout << "  ";
            std::cout << "     ";
            std::cout << "  ";
            if(!validString(currentToken)){
                std::cout << "PROBLEM!\n";
                exit(100);
            }
            try{
                outList.push_back(std::stoi(currentToken));
            }catch (std::invalid_argument){
                std::cout << "current token? " << currentToken << "\n";
            }
            currentToken.clear();
            idx = idx + 3;
        }
        currentToken.push_back(inputString[idx]);
    }
    for (idx; idx < inputString.size(); idx ++) {
        currentToken.push_back(inputString[idx]);
    }
    std::cout << "value_end: " << currentToken << "\t";
            if(!validString(currentToken)){
                std::cout << "PROBLEM!\n";
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
    if (inputFile.is_open()) {
        std::string line;

        while (std::getline(inputFile, line)) {
            if (line == "inNeigh") {
                std::getline(inputFile, line);
                graphObj.inNeigh = processIntToListDict(line);
            }
            else if (line == "outNeigh") {
                std::getline(inputFile, line);
                graphObj.outNeigh = processIntToListDict(line);
            }
            else if (line == "validNodes") {
                std::getline(inputFile, line);
                graphObj.validNodes = processList(line);
            }
        }
        inputFile.close();
    } else {
        std::cout << "Unable to open file: " << fileName << std::endl;
    }

    return graphObj;
}

int main() {
    std::string fileName = "example.txt";
    graphInfo graphObj = processFile(fileName);

    //print the struct to check
    std::cout << "InNeigh" << std::endl;
    for (auto& p : graphObj.inNeigh) {
        std::cout << p.first << ": ";
        for (auto& val : p.second) {
            std::cout << val << ",";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "OutNeigh" << std::endl;
    for (auto& p : graphObj.outNeigh) {
        std::cout << p.first << ": ";
        for (auto& val : p.second) {
            std::cout << val << ",";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "validNodes" << std::endl;
    for (auto& val : graphObj.validNodes) {
        std::cout << val << ",";
        std::cout << std::endl;
    }
    std::cout << std::endl;

    return 0;
}