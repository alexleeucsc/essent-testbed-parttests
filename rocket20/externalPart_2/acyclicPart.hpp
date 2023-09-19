#ifndef ACYCLICPART_H
#define ACYCLICPART_H

#include "Graph.hpp"
#include "mergeIsAcyclicTopo.hpp"
#include <set>
#include <unordered_set>
#include <cassert>
#include <iterator>
#include <iostream>
#include <unordered_map>
#include <map>
#include <vector>
#include <algorithm>
#include <functional>
#include <limits>


#include "Index.h"
#include "Timer.h"
// #define SHOW(X) std::cout << # X " = " << (X) << std::endl
// #define PRINTLIST(vec) do { \
//     std::cout << "Printing elements of the vector: "; \
//     for (const auto& element : vec) { \
//         std::cout << element << " "; \
//     } \
//     std::cout << std::endl; \
// } while(0)

// void testPrint( std::map<int, std::vector<int>> & m, int i )
// {
//   PRINTLIST( m[i] );
//   SHOW( m.find(i)->first );
// }

void testPrintVec( std::vector<int> & v, int i )
{
  PRINTLIST( v );
}

void checkMemberInSet(const std::set<int>& input_set, int member) {
    bool isFound = false;

    for (const int& element : input_set) {
        if (element == member) {
            isFound = true;
            break;
        }
    }

    //std::cout << std::boolalpha << isFound << std::endl;
}

// void checkMemberInVector(const std::vector<int>& vec, int member) {
//     bool isFound = false;

//     for (int i=0; i<vec.size(); i++) {
//         int element = vec[i];
//         if (element == member) {
//             isFound = true;
//             //std::cout << isFound << std::endl;
//         }
//     }
//     if(!isFound){
//         //std::cout << std::boolalpha << isFound << std::endl;
//     }
// }

bool checkMemberInVector_debug(const std::vector<int>& vec, int member) {
    bool isFound = false;

    for (const int& element : vec) {
        if (element == member) {
            return true;
        }
    }

    return false;
}

class AcyclicPart{
    public:
    graphInfo mg;
    std::vector<bool> excludeSet;
    std::vector<bool> clustersToKeep;

    //DEBUG FLAGS
    int debugMergeReqSize = 0;

    AcyclicPart(graphInfo mg, std::vector<int> excludeSetInts){
        this->mg = mg;
        int maxExcluded = *(max_element(excludeSetInts.begin(), excludeSetInts.end()));
        this->excludeSet.reserve(maxExcluded+1);
        for(int i=0; i<maxExcluded+1; i++){
            this->excludeSet.push_back(false);
        }
        for(int i=0; i<excludeSetInts.size(); i++){
            this->excludeSet[excludeSetInts[i]] = true;
        }
        this->clustersToKeep.reserve(mg.inNeigh.size());
        for(int i=0; i<mg.inNeigh.size(); i++){
            this->clustersToKeep.push_back(false);
        }
    }

    std::vector<int> findSmallParts(int smallPartCutoff) {
        std::vector<int> result;
        result.reserve(mg.mergeIDToMembers.size());
        //mg.mergeIDToMembers[0];
        for (const auto& pair : mg.mergeIDToMembers) {
            int id = pair.first;
            if (mg.mergeIDToMembers.find(id) != mg.mergeIDToMembers.end() &&
                mg.nodeSize(id) < smallPartCutoff &&
                !excludeSet[id] &&
                !clustersToKeep[id]) {
                result.push_back(id);
            }
        }
        std::sort(result.begin(), result.end());
        return result;
    }

    void mergeSingleInputPartsIntoParents(int smallPartCutoff = 20){
        std::vector<int> smallPartIDs = findSmallParts(smallPartCutoff);
        // //std::cout << "smallPartIDs\n";
        // for(int e : smallPartIDs){
        // //std::cout << e << " ";
        // }
        // //std::cout << "\n";
        // mg.outputMGMergeInfo_modfile("");
        // mg.outputGNeighbors_modfile("");
        std::vector<int> singleInputIDs;
        for (int id : smallPartIDs) {
            if (mg.inNeigh[id].size() == 1) {
                singleInputIDs.push_back(id);
            }
        }
        std::set<int> singleInputParents;
        for (int id : singleInputIDs) {
            const std::vector<int>& inNeigh = mg.inNeigh[id];
            singleInputParents.insert(inNeigh.begin(), inNeigh.end());
        }
        std::set<int> baseSingleInputIDs;
        std::set<int> singleInputParentsSet(singleInputParents.begin(), singleInputParents.end());
        for (int id : singleInputIDs) {
            if (singleInputParentsSet.find(id) == singleInputParentsSet.end()) {
                baseSingleInputIDs.insert(id);
            }
        }

        //debug 080523
        //std::cout << "baseSingleInputIDs\n";
        for(auto c : baseSingleInputIDs){
            //std::cout << c << ", ";
        }
        //std::cout << std::endl;

        for (int childID : baseSingleInputIDs) {
            const std::vector<int>& inNeigh = mg.inNeigh[childID];
            if (!inNeigh.empty()) {
                int parentID = inNeigh.front();
                if (!excludeSet[parentID] &&
                    !clustersToKeep[parentID]) {
                    mg.mergeGroups(parentID, std::vector<int>{childID});
                }
            }
        }
        if (baseSingleInputIDs.size() < singleInputIDs.size()){
            mergeSingleInputPartsIntoParents(smallPartCutoff);
        }
    }

    typedef int NodeID;
    typedef std::set<NodeID> Set;
    typedef std::vector<Set> Seq;

    Seq performMergesIfPossible(const Seq& mergesToConsider, bool printout = false) {
        Seq mergesMade;
        for (const auto& mergeReq : mergesToConsider) { 
            // if(debugMergeReqSize){
            //     std::cout << "mergeReq size:" << mergeReq.size() << std::endl;
            // }
            if(printout){
                std::cout << "m:\n";
                for(auto m:mergeReq){
                    std::cout << m << ", ";
                }
                std::cout << "\n";
            }
            assert(mergeReq.size() > 1);
            bool partsStillExist = true;
            for (const auto& id : mergeReq) {
                if (mg.mergeIDToMembers.find(id) == mg.mergeIDToMembers.end()) {
                    partsStillExist = false;
                    break;
                }
            }
            //bool isAcyclic = mg.mergeIsAcyclic(Set(mergeReq.begin(), mergeReq.end()));
            if (partsStillExist && mg.mergeIsAcyclic(Set(mergeReq.begin(), mergeReq.end()))) {
                // if(!std::all_of(mergeReq.begin(), mergeReq.end(), [&](NodeID id) {
                //     return !excludeSet[id];})){
                //         //std::cout << "error";
                //     }
                assert(std::all_of(mergeReq.begin(), mergeReq.end(), [&](NodeID id) {
                    return !excludeSet[id];
                }));
                //DEBUG 080223
                //mg.mergeGroups(*(mergeReq.begin()), std::vector<int>(std::next(mergeReq.begin(),1), mergeReq.end()));
                std::vector<int> mergeReqVec(mergeReq.begin(), mergeReq.end());
                int maxValMRV = *std::max_element(mergeReqVec.begin(), mergeReqVec.end());
                mergeReqVec.erase(std::remove(mergeReqVec.begin(), mergeReqVec.end(), maxValMRV), mergeReqVec.end());
                mg.mergeGroups(maxValMRV, mergeReqVec);

                mergesMade.push_back(mergeReq);
            }
        }
        return mergesMade;
    }
    Seq performMergesCheckFree(const Seq& mergesToConsider) {
        Seq mergesMade;
        for (const auto& mergeReq : mergesToConsider) { 
            if(debugMergeReqSize){
                std::cout << "mergeReq size:" << mergeReq.size() << std::endl;
            }
            assert(mergeReq.size() > 1);
            bool partsStillExist = true;
            for (const auto& id : mergeReq) {
                if (mg.mergeIDToMembers.find(id) == mg.mergeIDToMembers.end()) {
                    partsStillExist = false;
                    break;
                }
            }
            if (partsStillExist) {
                // if(!mg.mergeIsAcyclic(Set(mergeReq.begin(), mergeReq.end()))){
                //     std::cout<<"PROBLEM!!!\n";
                // }
                if(!std::all_of(mergeReq.begin(), mergeReq.end(), [&](NodeID id) {
                    return !excludeSet[id];})){
                        //std::cout << "error";
                    }
                assert(std::all_of(mergeReq.begin(), mergeReq.end(), [&](NodeID id) {
                    return !excludeSet[id];
                }));
                //DEBUG 080223
                //mg.mergeGroups(*(mergeReq.begin()), std::vector<int>(std::next(mergeReq.begin(),1), mergeReq.end()));
                std::vector<int> mergeReqVec(mergeReq.begin(), mergeReq.end());
                int maxValMRV = *std::max_element(mergeReqVec.begin(), mergeReqVec.end());
                mergeReqVec.erase(std::remove(mergeReqVec.begin(), mergeReqVec.end(), maxValMRV), mergeReqVec.end());
                mg.mergeGroups(maxValMRV, mergeReqVec);

                mergesMade.push_back(mergeReq);
            }
        }
        return mergesMade;
    }
    Seq performMergesIfPossibleLimited(const Seq& mergesToConsider) {
        Seq mergesMade;
        //process all mergeReqs, fill up mergeReqs not made to memoize on...
        Seq mergesLimited;
        for (const auto& mergeReq : mergesToConsider) { 
            if(debugMergeReqSize){
                std::cout << "mergeReq short size:" << mergeReq.size() << std::endl;
            }
            assert(mergeReq.size() > 1);
            bool partsStillExist = true;
            for (const auto& id : mergeReq) {
                if (mg.mergeIDToMembers.find(id) == mg.mergeIDToMembers.end()) {
                    partsStillExist = false;
                    break;
                }
            }
            std::tuple<bool,bool> isAcyclic_limited = mg.mergeIsAcyclicLimited(Set(mergeReq.begin(), mergeReq.end()));
            bool isAcyclic = std::get<0>(isAcyclic_limited);
            bool limited = std::get<1>(isAcyclic_limited);
            if (partsStillExist && limited){
                mergesLimited.push_back(mergeReq);
            }
            else if (partsStillExist && isAcyclic) {
                if(!std::all_of(mergeReq.begin(), mergeReq.end(), [&](NodeID id) {
                    return !excludeSet[id];})){
                        //std::cout << "error";
                    }
                assert(std::all_of(mergeReq.begin(), mergeReq.end(), [&](NodeID id) {
                    return !excludeSet[id];
                }));
                //DEBUG 080223
                //mg.mergeGroups(*(mergeReq.begin()), std::vector<int>(std::next(mergeReq.begin(),1), mergeReq.end()));
                std::vector<int> mergeReqVec(mergeReq.begin(), mergeReq.end());
                int maxValMRV = *std::max_element(mergeReqVec.begin(), mergeReqVec.end());
                mergeReqVec.erase(std::remove(mergeReqVec.begin(), mergeReqVec.end(), maxValMRV), mergeReqVec.end());
                mg.mergeGroups(maxValMRV, mergeReqVec);

                mergesMade.push_back(mergeReq);
            }
        }
        //build index
        unsigned seeds = 0, k = ~0u; 
        bool global = true;
        std::vector<std::vector<unsigned>> inNeigh_u;
        std::vector<std::vector<unsigned>> outNeigh_u;
        for( auto inNeighVec : mg.inNeigh){
            std::vector<unsigned> nextVec;
            for(auto n : inNeighVec){
                nextVec.push_back(n);
            }
            inNeigh_u.push_back(nextVec);
        }
        for( auto outNeighVec : mg.outNeigh){
            std::vector<unsigned> nextVec;
            for(auto n : outNeighVec){
                nextVec.push_back(n);
            }
            outNeigh_u.push_back(nextVec);
        }
        Timer t;
        t.start();
        Graph *g = new Graph(inNeigh_u, outNeigh_u);
        Index bm(g, seeds, k, global);
        bm.build();
        std::cout << "indexing time " << t.stop() << "\n";
        //DEBUG
        //bool debugBool1 = mg.traverseUntilIntersect({22928},{22928},{21747},0);
        //process all limited merges:
        for (const auto& mergeReq : mergesLimited) { 
            // if(debugMergeReqSize){
            //     std::cout << "mergeReq long size:" << mergeReq.size() << std::endl;
            // }
            // if(mergeReq.find(21747) != mergeReq.end() && mergeReq.find(21859) != mergeReq.end()){
            //     std::cout<<"merge to check\n";
            // }
            // if(mergeReq.find(21747) != mergeReq.end() && mergeReq.find(22928) != mergeReq.end()){
            //     std::cout<<"merge to check2\n";
            // }
            // if(checkMemberInMap(mg.mergeIDToMembers, 21747)){
            //     std::cout << "21747 is now a node\n";
            // }
            assert(mergeReq.size() > 1);
            bool partsStillExist = true;
            for (const auto& id : mergeReq) {
                if (mg.mergeIDToMembers.find(id) == mg.mergeIDToMembers.end()) {
                    partsStillExist = false;
                    break;
                }
            }
            bool isAcyclic = false;
            if(partsStillExist){
                isAcyclic = mg.mergeIsAcyclic_Indexed(Set(mergeReq.begin(), mergeReq.end()), bm);
                //isAcyclic = mg.mergeIsAcyclic(Set(mergeReq.begin(), mergeReq.end()));
            }
            if (partsStillExist && isAcyclic) {
                if(!std::all_of(mergeReq.begin(), mergeReq.end(), [&](NodeID id) {
                    return !excludeSet[id];})){
                        //std::cout << "error";
                    }
                assert(std::all_of(mergeReq.begin(), mergeReq.end(), [&](NodeID id) {
                    return !excludeSet[id];
                }));
                //DEBUG 080223
                //mg.mergeGroups(*(mergeReq.begin()), std::vector<int>(std::next(mergeReq.begin(),1), mergeReq.end()));
                std::vector<int> mergeReqVec(mergeReq.begin(), mergeReq.end());
                int maxValMRV = *std::max_element(mergeReqVec.begin(), mergeReqVec.end());
                mergeReqVec.erase(std::remove(mergeReqVec.begin(), mergeReqVec.end(), maxValMRV), mergeReqVec.end());
                mg.mergeGroups(maxValMRV, mergeReqVec);

                mergesMade.push_back(mergeReq);
            }
            //DEBUG
            // if(mergeReq.find(21747) != mergeReq.end()){
            //     std::cout<<"break2\n";
            // }
            // bool debugBool1 = mg.traverseUntilIntersect({22928},{22928},{21747},0);
            // if(!debugBool1){
            //     std::cout<<"break\n";
            // }
            //DEBUG
        }
        return mergesMade;
    }
    Seq timeMergesOnly(const Seq& mergesToConsider) {
        Seq mergesMade;
        for (const auto& mergeReq : mergesToConsider) { 
            // if(debugMergeReqSize){
            //     std::cout << "mergeReq size:" << mergeReq.size() << std::endl;
            // }
            std::cout << "m:\n";
            for(auto m:mergeReq){
                std::cout << m << ", ";
            }
            std::cout << "\n";
            assert(mergeReq.size() > 1);
            bool partsStillExist = true;
            for (const auto& id : mergeReq) {
                if (mg.mergeIDToMembers.find(id) == mg.mergeIDToMembers.end()) {
                    partsStillExist = false;
                    break;
                }
            }
            //bool isAcyclic = mg.mergeIsAcyclic(Set(mergeReq.begin(), mergeReq.end()));
            if (partsStillExist && mg.mergeIsAcyclic(Set(mergeReq.begin(), mergeReq.end()))) {
                // if(!std::all_of(mergeReq.begin(), mergeReq.end(), [&](NodeID id) {
                //     return !excludeSet[id];})){
                //         //std::cout << "error";
                //     }
                assert(std::all_of(mergeReq.begin(), mergeReq.end(), [&](NodeID id) {
                    return !excludeSet[id];
                }));
                //DEBUG 080223
                //mg.mergeGroups(*(mergeReq.begin()), std::vector<int>(std::next(mergeReq.begin(),1), mergeReq.end()));
                std::vector<int> mergeReqVec(mergeReq.begin(), mergeReq.end());
                int maxValMRV = *std::max_element(mergeReqVec.begin(), mergeReqVec.end());
                mergeReqVec.erase(std::remove(mergeReqVec.begin(), mergeReqVec.end(), maxValMRV), mergeReqVec.end());
                //mg.mergeGroups(maxValMRV, mergeReqVec);

                mergesMade.push_back(mergeReq);
            }
        }
        return mergesMade;
    }

    void checkTopo(std::vector<int>& topoSort, graphInfo& mg){
        int maxNodeVal = *std::max_element(topoSort.begin(), topoSort.end());
        std::vector<bool> nodeHasBeenChecked(maxNodeVal+1, false);
        std::queue<int> frontier_tmp;
        for(auto n : topoSort){
            for(auto inN : mg.inNeigh[n]){
                if(!nodeHasBeenChecked[inN]){
                    //std::cout << "error: topoSort wrong\n";
                    exit(111);
                }
                //nodeHasBeenChecked[n] = true;
            }
            nodeHasBeenChecked[n] = true;
        }
    }

    struct VectorHash {
        size_t operator()(const std::vector<int>& vec) const {
            std::hash<int> hasher;
            size_t seed = vec.size();
            for (const int value : vec) {
                seed ^= hasher(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };

    struct VectorEqual {
        bool operator()(const std::vector<int>& lhs, const std::vector<int>& rhs) const {
            return lhs == rhs;
        }
    };

    //std::map<std::vector<NodeID>, std::vector<NodeID>, VectorHash, VectorEqual> groupByFirst(const std::vector<std::pair<std::vector<NodeID>, NodeID>>& arr) {
    std::map<std::vector<NodeID>, std::vector<NodeID>> groupByFirst(const std::vector<std::pair<std::vector<NodeID>, NodeID>>& arr) {
        std::map<std::vector<NodeID>, std::vector<NodeID>> indicesMap;

        for (int i = 0; i < arr.size(); ++i) {
            std::vector<NodeID> value = arr[i].first;
            indicesMap[value].push_back(arr[i].second);
        }

        return indicesMap;
    }

    void mergeSmallSiblings(int smallPartCutoff = 10) {
        std::vector<NodeID> smallPartIDs = findSmallParts(smallPartCutoff);
        // mg.outputMGMergeInfo_modfile("");
        // mg.outputGNeighbors_modfile("");

        std::vector<std::pair<std::vector<NodeID>, NodeID>> inputsAndIDPairs;   //is the toSeq necessary?
        for (const auto& id : smallPartIDs) {
            std::vector<NodeID> inputsCanonicalized(mg.inNeigh[id].begin(), mg.inNeigh[id].end());
            std::sort(inputsCanonicalized.begin(), inputsCanonicalized.end());
            if(checkMemberInVector_debug(inputsCanonicalized, 19813) | checkMemberInVector_debug(inputsCanonicalized, 19840) | id == 19813 | id == 19840){
                //std::cout << "error?";
            }
            inputsAndIDPairs.emplace_back(inputsCanonicalized, id);
        }

        //std::map<std::vector<NodeID>, std::vector<NodeID>, VectorHash, VectorEqual> inputsToSiblings = groupByFirst(inputsAndIDPairs);
        std::map<std::vector<NodeID>, std::vector<NodeID>> inputsToSiblings = groupByFirst(inputsAndIDPairs);

        std::vector<std::set<int>> mergesToConsider;
        for (const auto& inputSiblings : inputsToSiblings) {
            const std::vector<NodeID>& inputIDs = inputSiblings.first;
            const std::vector<NodeID>& siblingIDs = inputSiblings.second;
            //debug
            if(checkMemberInVector_debug(siblingIDs, 19813)){
                //std::cout << "error?";
            }

            if (siblingIDs.size() > 1) {
                mergesToConsider.push_back(std::set<int>(siblingIDs.begin(), siblingIDs.end()));
            }
        }
        //logger.info("  attempting to merge " + std::to_string(mergesToConsider.size()) + " groups of small siblings");

        //----------072423 DEBUG----------
        // std::ofstream myfile_merge2con;
        // myfile_merge2con.open("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/072423_mergesToConsider_cpp",
        //     std::ios_base::app);
        // myfile_merge2con << "mergesToConsider\n";
        // for( const auto &p : mergesToConsider){
        //     for(const auto &member : p){
        //         myfile_merge2con << member;
        //         myfile_merge2con << "<2>";
        //     }
        //     myfile_merge2con << "<1>";
        // }
        // myfile_merge2con << "\n";
        // myfile_merge2con.close();
        std::sort(mergesToConsider.begin(), mergesToConsider.end() );
        std::cout << "mergeSmallSiblings: found parts:" << mergesToConsider.size() << "out of " << smallPartIDs.size() << "\n";
        std::vector<std::set<int>> mergesMade = performMergesIfPossible(mergesToConsider);
        //std::cout << "\n";
        //std::vector<std::set<int>> mergesMade = performMergesIfPossibleTopo(mergesToConsider);
        if (!mergesMade.empty()) {
            mergeSmallSiblings(smallPartCutoff);
        }
    }

    typedef std::map<NodeID, Set> NeighborsDict;

    void mergeSmallParts(int smallPartCutoff = 20, double mergeThreshold = 0.5) {
        std::vector<int> smallPartIDs = findSmallParts(smallPartCutoff);
        // mg.outputMGMergeInfo_modfile("");
        // mg.outputGNeighbors_modfile("");

        Seq mergesToConsider;
        //std::cout << "\nmergesToConsider_choices\n";// 072723 DEBUG
        for (const auto& id : smallPartIDs) {
            double numInputs = static_cast<double>(mg.inNeigh[id].size());
            
            // debug
            // Set siblings = mg.outNeigh(id);
            // siblings.insert(mg.inNeigh(id).begin(), mg.inNeigh(id).end());
            // siblings.erase(id);

            std::vector<NodeID> orderConstrSibs;
            for (const auto& neighbor : mg.inNeigh.at(id)) {
                for (const auto& outNeighbor : mg.outNeigh.at(neighbor)) { //had to add mg.
                    if (outNeighbor != id &&
                        !excludeSet[outNeighbor] &&
                        !clustersToKeep[outNeighbor] &&
                        outNeighbor < id) {
                        orderConstrSibs.push_back(outNeighbor);
                    }
                }
            }

            Set myInputSet(mg.inNeigh[id].begin(), mg.inNeigh[id].end());

            std::vector<std::pair<double, NodeID>> sibsScored;
            for (const auto& sibID : orderConstrSibs) {
                int count = 0;
                const std::vector<int>& inNeighSet = mg.inNeigh.at(sibID);
                for (const auto& neighbor : inNeighSet) {
                    if (myInputSet.find(neighbor) != myInputSet.end()) {
                        count++;
                    }
                }
                double score = static_cast<double>(count) / numInputs;
                sibsScored.emplace_back(score, sibID);
            }

            std::vector<std::pair<double, NodeID>> choices;
            for (const auto& entry : sibsScored) {
                if (entry.first >= mergeThreshold) {
                    choices.push_back(entry);
                }
            }

            std::sort(choices.begin(), choices.end(), std::greater<std::pair<double, NodeID>>());
            for(auto c : choices){// 072723 DEBUG
                //std::cout << c.second << "<2>";
            }
            //std::cout << "<1>";// 072723 DEBUG
            NodeID topChoice = -1;
            //std::cout << "mSP-acyclicCheck:\n";
            for (const auto& choice : choices) {
                //if (mg.mergeIsAcyclic(choice.second, id)) {
                    topChoice = choice.second;
                    break;
                //}
            }
            //std::cout << "\n";

            if (topChoice != -1) {
                mergesToConsider.push_back({topChoice, id});
            }
        }
        //std::cout << "\n"; // 072723 DEBUG
        // 072623 DEBUG
        //logger.info("  of " + std::to_string(smallPartIDs.size()) + " small parts " + std::to_string(mergesToConsider.size()) + " worth merging");
        ////std::cout << "\nmergeSmallParts_mergesToConsider\n";
        // for(auto mergeGroup: mergesToConsider){
        //     for(auto m : mergeGroup){
        //         //std::cout << m << "<2>";
        //     }
        //     //std::cout << "<1>";
        // }
        // //std::cout << "\n";
        std::cout << "mergeSmallParts: found parts:" << mergesToConsider.size() << "out of " << smallPartIDs.size() << "\n";
        //Seq mergesMade = performMergesIfPossibleLimited(mergesToConsider);

        std::sort(mergesToConsider.begin(), mergesToConsider.end() );
        //Seq mergesMade = performMergesIfPossible(mergesToConsider);
        Seq mergesMade = performMergesIfPossibleLimited(mergesToConsider);
        //Seq mergesMade = performMergesCheckFree(mergesToConsider);
        //std::cout << "\n";
        if (!mergesMade.empty()) {
            mergeSmallParts(smallPartCutoff, mergeThreshold);
        }
    }



void mergeSmallPartsDown(int smallPartCutoff = 20) {
    std::vector<int> smallPartIDs = findSmallParts(smallPartCutoff);

    // mg.outputMGMergeInfo_modfile("");
    // mg.outputGNeighbors_modfile("");

    Seq mergesToConsider;
    for (const auto& id : smallPartIDs) {
        std::vector<int> mergeableChildren;
        for (const auto& childID : mg.outNeigh.at(id)) {
            if (
                //mg.mergeIsAcyclic(id, childID) &&
                !excludeSet[childID] &&
                !clustersToKeep[childID]
                )
                {
                    mergeableChildren.push_back(childID);
            }
        }
        //std::cout << "\n";

        if (!mergeableChildren.empty()) {
            std::vector<NodeID> orderedByEdgesRemoved(mergeableChildren.begin(), mergeableChildren.end());
            // Seq tmpVec1({id, childID1});
            // Seq tmpVec2({id, childID2});
            //DEBUG 080723
            std::sort(orderedByEdgesRemoved.begin(), orderedByEdgesRemoved.end(),
                [&](NodeID childID1, NodeID childID2) {
                    std::vector<NodeID> tmpVec1 = {id, childID1};
                    std::vector<NodeID> tmpVec2 = {id, childID2};
                    return mg.numEdgesRemovedByMerge(tmpVec1) < mg.numEdgesRemovedByMerge(tmpVec2);
                });
            std::sort(orderedByEdgesRemoved.begin(), orderedByEdgesRemoved.end());
            std::sort(orderedByEdgesRemoved.begin(), orderedByEdgesRemoved.end(),
                [&](NodeID childID1, NodeID childID2) {
                    std::vector<NodeID> tmpVec1 = {id, childID1};
                    std::vector<NodeID> tmpVec2 = {id, childID2};
                    return mg.numEdgesRemovedByMerge(tmpVec1) < mg.numEdgesRemovedByMerge(tmpVec2);
                });

            
            //std::sort(orderedByEdgesRemoved.begin(), orderedByEdgesRemoved.end()); bug?

            NodeID topChoice = orderedByEdgesRemoved.back();
            mergesToConsider.push_back({id, topChoice});
        }
    }

    //std::cout << "\nmergeSmallPartsDown_mergesToConsider\n";
    for(auto mergeGroup: mergesToConsider){
        for(auto m : mergeGroup){
            //std::cout << m << "<2>";
        }
        //std::cout << "<1>";
    }
    //std::cout << "\n";

    //debug 080523
    //std::cout << "mergeSmallPartsDown_mergesToConsider\n";
    for(auto mgroup : mergesToConsider){
        for(auto c : mgroup){
            //std::cout << c << "<2>";
        }
        //std::cout << "<1>";
    }
    //std::cout << std::endl;

    //logger.info("  of " + std::to_string(smallPartIDs.size()) + " small parts " + std::to_string(mergesToConsider.size()) + " worth merging down");
    std::cout << "mergeSmallPartsDown: found parts:" << mergesToConsider.size() << "out of " << smallPartIDs.size() << "\n";
    Seq mergesMade = performMergesIfPossible(mergesToConsider);
    //std::cout << "\n";
    if (!mergesMade.empty()) {
        mergeSmallPartsDown(smallPartCutoff);
    }
}


void mergeSmallPartsLayered(int smallPartCutoff = 20, double mergeThreshold = 0.5) {
    std::vector<int> smallPartIDs = findSmallParts(smallPartCutoff);
    std::tuple<std::vector<std::vector<int>>, std::vector<int>>  layerList_dists = mg.layerList();
    std::vector<std::vector<int>> layerList = std::get<0>(layerList_dists);
    std::vector<int> dists = std::get<1>(layerList_dists);
    //1: print out average distance to neighbors
    //std::cout << "ave dist\n";
    // for(int n=0; n<mg.outNeigh.size(); n++){
    //     int totalDist = 0;
    //     for(auto d : mg.outNeigh[n]){
    //         totalDist = totalDist + (dists[d] - dists[n]);
    //     }
    //     float aveDist = float(totalDist) / float(mg.outNeigh[n].size());
    //     //std::cout << aveDist << ", ";
    // }
    //std::cout << "\n";
    //2: iter over each small part and check how many small part neighbors it can merge with
    //std::cout << "neighbor count\n";
    Seq mergesToConsider;
    //std::cout << "smallPartIDs" << smallPartIDs.size() << "\n";
        int orderConstrSibsTotal = 0;
        clock_t begin = clock();
        int timeBlock1 = 0;
        int timeBlock2 = 0;
        int timeBlock3 = 0;
        int timeBlock4 = 0;
        int timeBlock5 = 0;
        for (const auto& id : smallPartIDs) {
            double numInputs = static_cast<double>(mg.inNeigh[id].size());
            
            // debug
            // Set siblings = mg.outNeigh(id);
            // siblings.insert(mg.inNeigh(id).begin(), mg.inNeigh(id).end());
            // siblings.erase(id);
            clock_t beginp = clock();
            std::vector<NodeID> orderConstrSibs;
            for (const auto& neighbor : mg.inNeigh.at(id)) {
                for (const auto& outNeighbor : mg.outNeigh.at(neighbor)) { //had to add mg.
                    if (outNeighbor != id &&
                        !excludeSet[outNeighbor] &&
                        !clustersToKeep[outNeighbor] &&
                        outNeighbor < id &&
                        dists[outNeighbor] == dists[id] ) {
                        orderConstrSibs.push_back(outNeighbor);
                    }
                }
            }
            if(orderConstrSibs.size() == 0){
                continue;
            }
            orderConstrSibsTotal = orderConstrSibsTotal + orderConstrSibs.size();
            Set myInputSet(mg.inNeigh[id].begin(), mg.inNeigh[id].end());
            clock_t endp = clock();
            timeBlock1 = timeBlock1 + endp-beginp;

            beginp = clock();
            std::vector<std::pair<double, NodeID>> sibsScored;
            for (const auto& sibID : orderConstrSibs) {
                int count = 0;
                const std::vector<int>& inNeighSet = mg.inNeigh.at(sibID);
                for (const auto& neighbor : inNeighSet) {
                    if (myInputSet.find(neighbor) != myInputSet.end()) {
                        count++;
                    }
                }
                double score = static_cast<double>(count) / numInputs;
                sibsScored.emplace_back(score, sibID);
            }
            endp = clock();
            timeBlock2 = timeBlock2 + endp-beginp;

            beginp = clock();
            std::vector<std::pair<double, NodeID>> choices;
            for (const auto& entry : sibsScored) {
                if (entry.first >= mergeThreshold) {
                    choices.push_back(entry);
                }
            }
            endp = clock();
            timeBlock3 = timeBlock3 + endp-beginp;

            beginp = clock();
            std::sort(choices.begin(), choices.end(), std::greater<std::pair<double, NodeID>>());
            for(auto c : choices){// 072723 DEBUG
                //std::cout << c.second << "<2>";
            }
            //std::cout << "<1>";// 072723 DEBUG
            NodeID topChoice = -1;
            //std::cout << "mSP-acyclicCheck:\n";
            for (const auto& choice : choices) {
                //if (mg.mergeIsAcyclic(choice.second, id)) {
                    topChoice = choice.second;
                    break;
                //}
            }
            //std::cout << "\n";
            endp = clock();
            timeBlock4 = timeBlock4 + endp-beginp;

            beginp = clock();
            if (topChoice != -1) {
                // if(!mg.mergeIsAcyclic(topChoice, id)){
                //     std::cout<<"PROBLEM!!!\n";
                // }
                mergesToConsider.push_back({topChoice, id});
            }
            endp = clock();
            timeBlock5 = timeBlock5 + endp-beginp;
        }
        clock_t end = clock();
        double elapsed_secs = double(end - begin);
        std::cout << "\tgather 1:" << elapsed_secs << "\n";
        std::cout << "\ttimeBlock1:" << timeBlock1 << "\n";
        std::cout << "\ttimeBlock2:" << timeBlock2 << "\n";
        std::cout << "\ttimeBlock3:" << timeBlock3 << "\n";
        std::cout << "\ttimeBlock4:" << timeBlock4 << "\n";
        std::cout << "\ttimeBlock5:" << timeBlock5 << "\n";
        std::cout << "orderConstrSibs size: " << orderConstrSibsTotal <<"\n";
        begin = clock();
    Seq mergesMade = performMergesCheckFree(mergesToConsider);
        end = clock();
        elapsed_secs = double(end - begin);
        std::cout << "\tmerge 1:" << elapsed_secs << "\n";
    //Seq mergesMade = performMergesIfPossible(mergesToConsider);
    std::cout << "mergeLayerList: found parts:" << mergesToConsider.size() << "out of " << smallPartIDs.size() << "\n";
    if (!mergesMade.empty()) {
        mergeSmallPartsLayered(smallPartCutoff, mergeThreshold);
    }   
}




    void smallSiblingsTimeTest(int smallPartCutoff = 20, double mergeThreshold = 0.5) {
        //std::vector<int> smallPartIDs = findSmallParts(smallPartCutoff);
        // mg.outputMGMergeInfo_modfile("");
        // mg.outputGNeighbors_modfile("");

        Seq mergesToConsider;
        //std::cout << "\nmergesToConsider_choices\n";// 072723 DEBUG
        std::cout << "smallParts timetest\n";
        for (auto id_pair : mg.mergeIDToMembers) {
            auto id = id_pair.first;

            double numInputs = static_cast<double>(mg.inNeigh[id].size());
            
            // debug
            // Set siblings = mg.outNeigh(id);
            // siblings.insert(mg.inNeigh(id).begin(), mg.inNeigh(id).end());
            // siblings.erase(id);

            std::vector<NodeID> orderConstrSibs;
            for (const auto& neighbor : mg.inNeigh.at(id)) {
                for (const auto& outNeighbor : mg.outNeigh.at(neighbor)) { //had to add mg.
                    if (outNeighbor != id &&
                        !excludeSet[outNeighbor] &&
                        !clustersToKeep[outNeighbor] &&
                        outNeighbor < id) {
                        orderConstrSibs.push_back(outNeighbor);
                    }
                }
            }

            Set myInputSet(mg.inNeigh[id].begin(), mg.inNeigh[id].end());

            std::vector<std::pair<double, NodeID>> sibsScored;
            for (const auto& sibID : orderConstrSibs) {
                int count = 0;
                const std::vector<int>& inNeighSet = mg.inNeigh.at(sibID);
                for (const auto& neighbor : inNeighSet) {
                    if (myInputSet.find(neighbor) != myInputSet.end()) {
                        count++;
                    }
                }
                double score = static_cast<double>(count) / numInputs;
                sibsScored.emplace_back(score, sibID);
            }

            std::vector<std::pair<double, NodeID>> choices;
            for (const auto& entry : sibsScored) {
                if (entry.first >= mergeThreshold) {
                    choices.push_back(entry);
                }
            }

            std::sort(choices.begin(), choices.end(), std::greater<std::pair<double, NodeID>>());
            for(auto c : choices){// 072723 DEBUG
                //std::cout << c.second << "<2>";
            }
            //std::cout << "<1>";// 072723 DEBUG
            NodeID topChoice = -1;
            //std::cout << "mSP-acyclicCheck:\n";
            for (const auto& choice : choices) {
                if (mg.mergeIsAcyclic(choice.second, id)) {
                    topChoice = choice.second;
                    break;
                }
            }
            //std::cout << "\n";

            if (topChoice != -1) {
                mergesToConsider.push_back({topChoice, id});
            }
        }
        //std::cout << "\n"; // 072723 DEBUG
        // 072623 DEBUG
        //logger.info("  of " + std::to_string(smallPartIDs.size()) + " small parts " + std::to_string(mergesToConsider.size()) + " worth merging");
        ////std::cout << "\nmergeSmallParts_mergesToConsider\n";
        // for(auto mergeGroup: mergesToConsider){
        //     for(auto m : mergeGroup){
        //         //std::cout << m << "<2>";
        //     }
        //     //std::cout << "<1>";
        // }
        // //std::cout << "\n";
        Seq mergesMade = timeMergesOnly(mergesToConsider);
        std::cout << "smallParts timetest\n";
    }

};

#endif