#include "parseGraph.hpp"

int main() {
    std::string fileName = "example.txt";
    graphInfo graphObj = processFile(fileName);

    //print the struct to check
    std::cout << "InNeigh" << std::endl;
    for (int i=0; i<graphObj.inNeigh.size(); i++) {
        std::cout << i << ": ";
        for (auto& val : graphObj.inNeigh[i]) {
            std::cout << val << ",";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "OutNeigh" << std::endl;
    for (int i=0; i<graphObj.outNeigh.size(); i++) {
        std::cout << i << ": ";
        for (auto& val : graphObj.outNeigh[i]) {
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