#ifndef MFFC_H
#define MFFC_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <hash_map>
#include <queue>
#include <set>

#include <unordered_map>
#include <map>

#include <stdio.h>
#include <time.h>
#include "Graph.hpp"

// class graphInfo{
//     public:
//     std::vector<std::vector<int>> inNeigh;
//     std::vector<std::vector<int>> outNeigh;
//     std::vector<int> validNodes;
//     graphInfo(int nodeCount, int validCount){
//         inNeigh.resize(nodeCount, std::vector<int>());
//         outNeigh.resize(nodeCount, std::vector<int>());
//         validNodes.resize(validCount,0);
//     }
//     void addEdge(int src, int dest){
//         inNeigh[dest].push_back(src);
//         outNeigh[src].push_back(dest);
//     }
// };

std::vector<int> getMFFCs(graphInfo graphObj, std::vector<int> excluded){
    std::ofstream myfile;
    myfile.open ("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/essent/mffcdebugdumpFromCpp");
    std::vector<int> mffc(graphObj.inNeigh.size(), -1);
    std::set<int> parentsOfExcluded;
    std::set<int> newMFFCSeeds;
    myfile << "parents of excluded:\n";
    for(auto exc : excluded){
        mffc[exc] = -2;
        for(auto p : graphObj.inNeigh[exc]){
            parentsOfExcluded.insert(p);
            myfile << p << " ";
        }
    }
    myfile << "\n";
    for(auto p : parentsOfExcluded){
        if(mffc[p] == -1){
            newMFFCSeeds.insert(p);
        }
    }
    for(int node=0; node<graphObj.outNeigh.size(); node++){
        if(mffc[node] == -1 and graphObj.outNeigh[node].size()==0){
            newMFFCSeeds.insert(node);
            // myfile << node << " ";
        }
    }
    myfile << "newMFFCSeeds:\n";
    for(auto newMFFCSeed : newMFFCSeeds){
        myfile << newMFFCSeed << " ";
    }
    myfile << "\n\n";
    //TODO: visited can be excluded, so these still need to be completed and added to newMFFCSeeds
    std::vector<int> visited;
    std::vector<int> fringe;
    std::vector<int> unvisitedFringe;
    //frontier 1
    
    while(newMFFCSeeds.size()>0){
        std::cout << "newMFFCSeeds.size(): " << newMFFCSeeds.size() << "\n";
        for(auto s : newMFFCSeeds){
            myfile << s << " ";
        }
        myfile << "\n";
        // for(int node2=0; node2<newMFFCSeeds.size(); node2++){
        //     //std::cout << "node2: " << node2 << "  newMFFCSeeds.size(): " << newMFFCSeeds.size() << "\n";
        //     mffc[newMFFCSeeds[node2]] = newMFFCSeeds[node2];
        // }
        for(auto node2 : newMFFCSeeds){
            //std::cout << "node2: " << node2 << "  newMFFCSeeds.size(): " << newMFFCSeeds.size() << "\n";
            mffc[node2] = node2;
        }
        //frontier 2
        //basically: nodes are either added to new MFFC or they are added to frontier 1
        std::deque<int> fringeFront;
        fringeFront.clear();
        //fringeFront = newMFFCSeeds;
        for(auto node : newMFFCSeeds){
            fringeFront.push_back(node);
            //mffc[node] = node;
        }
        newMFFCSeeds.clear();
        do{
            //for(auto frontNode : newMFFCSeeds){ BUG
            //while(newMFFCSeeds.size()>0){
            int frontNode = fringeFront.front();
            fringeFront.pop_front();
            bool valid = false;
            std::vector<int> parents = graphObj.inNeigh[frontNode]; //is this efficient?
            for(int parent : parents){
                if(mffc[parent] == -1){
                    bool uniqueMFFCs = true;
                    for (auto outNeigh : graphObj.outNeigh[parent]){
                        if (mffc[outNeigh] != mffc[graphObj.outNeigh[parent][0]]){uniqueMFFCs = false; break;}
                    }
                    if (uniqueMFFCs and graphObj.outNeigh[parent].size()>0 and mffc[graphObj.outNeigh[parent][0]] != -1){
                        fringeFront.push_back(parent);
                        mffc[parent] = mffc[graphObj.outNeigh[parent][0]];
                        //071423 : this is probably not too efficient: 
                        if(newMFFCSeeds.find(parent) != newMFFCSeeds.end()){
                            newMFFCSeeds.erase(parent);
                        }
                        //std::cout << parent << " & " << graphObj.outNeigh[parent][0] << "\n";
                    }else{
                        //if(std::find(newMFFCSeeds.begin(), newMFFCSeeds.end(), parent) == newMFFCSeeds.end()){
                        if(newMFFCSeeds.find(parent) == newMFFCSeeds.end()){
                            newMFFCSeeds.insert(parent);
                        }
                        //std::cout << "front add:" << parent << "\n";
                    }
                }
            }
            //}
            // std::cout << "fringefront: ";
            // for(auto i:fringeFront){
            //     std::cout << i << ",  ";
            // }
            // std::cout << "\n";
        }while(fringeFront.size()>0);
    }
    myfile.close();
    for(auto exc : excluded){
        mffc[exc] = exc;
    }
    return mffc;
}

#endif