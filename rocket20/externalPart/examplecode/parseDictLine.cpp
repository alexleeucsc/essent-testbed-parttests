#include <iostream>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <string>

std::unordered_map<int, std::vector<int>> processIntToListDict(std::string inputString){
    std::unordered_map<int, std::vector<int>> hashmap;
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

std::vector<int> processList(std::string inputString){
    std::vector<int> outList;
    std::string currentToken;
    int currentKey;
    int currentTokenMode = 0;
    int idx = 0;
    for (idx; idx < inputString.size()-2; idx ++) {
        if(inputString.substr(idx,3) == "<1>"){
            std::cout << "value: " << currentToken << "\t";
            outList.push_back(std::stoi(currentToken));
            currentToken.clear();
            idx = idx + 3;
        }
        currentToken.push_back(inputString[idx]);
    }
    for (idx; idx < inputString.size(); idx ++) {
        currentToken.push_back(inputString[idx]);
    }
    std::cout << "value_end: " << currentToken << "\t";
    outList.push_back(std::stoi(currentToken));
    currentToken.clear();
    idx = idx + 3;
    return outList;
}

int main() {
    std::string inputString = "1<2>4<3>5<3>6<1>2<2>7<3>8<3>9<3>10<1>3<2>11<3>12<3>13<3>14";
    std::unordered_map<int, std::vector<int>> hashmap = processIntToListDict(inputString);
    std::cout << "\n";
    for(const auto& elem : hashmap)
    {
        std::cout << elem.first << ":";
        for(const auto& elem2 : elem.second)
        {
            std::cout << elem2 << ",";
        }
        std::cout << "\n";
    }
    std::string inputString2 = "1<1>2<1>333<1>4<1>5<1>66<1>77<1>88";
    std::vector<int> outList = processList(inputString2);
    std::cout << "\n";
    for(const auto& elem : outList)
    {
        std::cout << elem << ",";
    }
    std::cout << "\n";
    return 0;
}