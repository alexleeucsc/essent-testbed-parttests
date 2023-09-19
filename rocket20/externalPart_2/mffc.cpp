#include "mffc.hpp"

int main(){
    graphInfo graphObj(10,10);
    std::vector<int> excluded;
    // test 1
    // graphObj.addEdge(0,1);
    // graphObj.addEdge(1,2);
    // graphObj.addEdge(3,4);
    // graphObj.addEdge(4,5);
    // graphObj.addEdge(4,2);
    // graphObj.addEdge(6,7);
    graphObj.addEdge(1,0);
    graphObj.addEdge(2,0);
    graphObj.addEdge(3,0);
    graphObj.addEdge(4,1);
    graphObj.addEdge(4,2);
    graphObj.addEdge(5,4);
    graphObj.addEdge(6,3);
    graphObj.addEdge(6,7);
    graphObj.addEdge(7,8);
    graphObj.addEdge(7,9);
    std::vector<int> mffc = getMFFCs(graphObj, excluded);
    std::cout << "mffc: ";
    for(auto i:mffc){
        std::cout << i << ",  ";
    }
    std::cout << "\n";    
    return 0;
}