#ifndef ACYCLICPART_H
#define ACYCLICPART_H

#include "Graph.hpp"
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

void checkMemberInSet(const std::unordered_set<int>& input_set, int member) {
    bool isFound = false;

    for (const int& element : input_set) {
        if (element == member) {
            isFound = true;
            break;
        }
    }

    std::cout << std::boolalpha << isFound << std::endl;
}

void checkMemberInVector(const std::vector<int>& vec, int member) {
    bool isFound = false;

    for (const int& element : vec) {
        if (element == member) {
            isFound = true;
            break;
        }
    }

    std::cout << std::boolalpha << isFound << std::endl;
}

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
    std::set<int> excludeSet;
    std::set<int> clustersToKeep;
    AcyclicPart(graphInfo mg, std::set<int> excludeSet){
        this->mg = mg;
        this->excludeSet = excludeSet;
    }

    std::vector<int> findSmallParts(int smallPartCutoff) {
        std::vector<int> result;
        //mg.mergeIDToMembers[0];
        for (const auto& pair : mg.mergeIDToMembers) {
            int id = pair.first;
            if (mg.mergeIDToMembers.count(id) && mg.nodeSize(id) < smallPartCutoff &&
                excludeSet.count(id) == 0 && clustersToKeep.count(id) == 0) {
                result.push_back(id);
            }
        }
        std::sort(result.begin(), result.end());
        return result;
    }

    void mergeSingleInputPartsIntoParents(int smallPartCutoff = 20){
        std::vector<int> smallPartIDs = findSmallParts(smallPartCutoff);
        // std::cout << "smallPartIDs\n";
        // for(int e : smallPartIDs){
        // std::cout << e << " ";
        // }
        // std::cout << "\n";
        // mg.outputMGMergeInfo_modfile("");
        // mg.outputGNeighbors_modfile("");
        std::vector<int> singleInputIDs;
        for (int id : smallPartIDs) {
            if (mg.inNeigh[id].size() == 1) {
                singleInputIDs.push_back(id);
            }
        }
        std::unordered_set<int> singleInputParents;
        for (int id : singleInputIDs) {
            const std::vector<int>& inNeigh = mg.inNeigh[id];
            singleInputParents.insert(inNeigh.begin(), inNeigh.end());
        }
        std::unordered_set<int> baseSingleInputIDs;
        std::unordered_set<int> singleInputParentsSet(singleInputParents.begin(), singleInputParents.end());
        for (int id : singleInputIDs) {
            if (singleInputParentsSet.count(id) == 0) {
                baseSingleInputIDs.insert(id);
            }
        }

        //debug 080523
        std::cout << "baseSingleInputIDs\n";
        for(auto c : baseSingleInputIDs){
            std::cout << c << ", ";
        }
        std::cout << std::endl;

        for (int childID : baseSingleInputIDs) {
            const std::vector<int>& inNeigh = mg.inNeigh[childID];
            if (!inNeigh.empty()) {
                int parentID = inNeigh.front();
                if (!excludeSet.count(parentID) && !clustersToKeep.count(parentID)) {
                    mg.mergeGroups(parentID, std::vector<int>{childID});
                }
            }
        }
        if (baseSingleInputIDs.size() < singleInputIDs.size()){
            mergeSingleInputPartsIntoParents(smallPartCutoff);
        }
    }

    typedef int NodeID;
    typedef std::unordered_set<NodeID> Set;
    typedef std::vector<Set> Seq;

    Seq performMergesIfPossible(const Seq& mergesToConsider) {
        Seq mergesMade;
        for (const auto& mergeReq : mergesToConsider) { 
            assert(mergeReq.size() > 1);
            bool partsStillExist = true;
            for (const auto& id : mergeReq) {
                if (mg.mergeIDToMembers.find(id) == mg.mergeIDToMembers.end()) {
                    partsStillExist = false;
                    break;
                }
            }
            if (partsStillExist && mg.mergeIsAcyclic(Set(mergeReq.begin(), mergeReq.end()))) {
                if(!std::all_of(mergeReq.begin(), mergeReq.end(), [&](NodeID id) {
                    return excludeSet.find(id) == excludeSet.end();})){
                        std::cout << "error";
                    }
                assert(std::all_of(mergeReq.begin(), mergeReq.end(), [&](NodeID id) {
                    return excludeSet.find(id) == excludeSet.end();
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

    std::unordered_map<std::vector<NodeID>, std::vector<NodeID>, VectorHash, VectorEqual> groupByFirst(const std::vector<std::pair<std::vector<NodeID>, NodeID>>& arr) {
        std::unordered_map<std::vector<NodeID>, std::vector<NodeID>, VectorHash, VectorEqual> indicesMap;

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
                std::cout << "error?";
            }
            inputsAndIDPairs.emplace_back(inputsCanonicalized, id);
        }

        std::unordered_map<std::vector<NodeID>, std::vector<NodeID>, VectorHash, VectorEqual> inputsToSiblings = groupByFirst(inputsAndIDPairs);

        std::vector<std::unordered_set<int>> mergesToConsider;
        for (const auto& inputSiblings : inputsToSiblings) {
            const std::vector<NodeID>& inputIDs = inputSiblings.first;
            const std::vector<NodeID>& siblingIDs = inputSiblings.second;
            //debug
            if(checkMemberInVector_debug(siblingIDs, 19813)){
                std::cout << "error?";
            }

            if (siblingIDs.size() > 1) {
                mergesToConsider.push_back(std::unordered_set<int>(siblingIDs.begin(), siblingIDs.end()));
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

        std::vector<std::unordered_set<int>> mergesMade = performMergesIfPossible(mergesToConsider);

        if (!mergesMade.empty()) {
            mergeSmallSiblings(smallPartCutoff);
        }
    }

    typedef std::unordered_map<NodeID, Set> NeighborsDict;

    void mergeSmallParts(int smallPartCutoff = 20, double mergeThreshold = 0.5) {
        std::vector<int> smallPartIDs = findSmallParts(smallPartCutoff);
        mg.outputMGMergeInfo_modfile("");
        mg.outputGNeighbors_modfile("");

        Seq mergesToConsider;
        std::cout << "\nmergesToConsider_choices\n";// 072723 DEBUG
        for (const auto& id : smallPartIDs) {
            double numInputs = static_cast<double>(mg.inNeigh[id].size());
            
            // debug
            // Set siblings = mg.outNeigh(id);
            // siblings.insert(mg.inNeigh(id).begin(), mg.inNeigh(id).end());
            // siblings.erase(id);
            Set siblings;
            for (const auto& neighbor : mg.inNeigh.at(id)) {
                for (const auto& outNeighbor : mg.outNeigh.at(neighbor)) { //had to add mg.
                    if (outNeighbor != id) {
                        siblings.insert(outNeighbor);
                    }
                }
            }

            Set legalSiblings;
            for (const auto& sibID : siblings) {
                if (!excludeSet.count(sibID) && !clustersToKeep.count(sibID)) {
                    legalSiblings.insert(sibID);
                }
            }

            Set orderConstrSibs;
            for (const auto& sibID : legalSiblings) {
                if (sibID < id) {
                    orderConstrSibs.insert(sibID);
                }
                // orderConstrSibs.insert(sibID);
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
                std::cout << c.second << "<2>";
            }
            std::cout << "<1>";// 072723 DEBUG
            NodeID topChoice = -1;
            for (const auto& choice : choices) {
                if (mg.mergeIsAcyclic(choice.second, id)) {
                    topChoice = choice.second;
                    break;
                }
            }

            if (topChoice != -1) {
                mergesToConsider.push_back({topChoice, id});
            }
        }
        std::cout << "\n"; // 072723 DEBUG
        // 072623 DEBUG
        //logger.info("  of " + std::to_string(smallPartIDs.size()) + " small parts " + std::to_string(mergesToConsider.size()) + " worth merging");
        //std::cout << "\nmergeSmallParts_mergesToConsider\n";
        // for(auto mergeGroup: mergesToConsider){
        //     for(auto m : mergeGroup){
        //         std::cout << m << "<2>";
        //     }
        //     std::cout << "<1>";
        // }
        // std::cout << "\n";
        Seq mergesMade = performMergesIfPossible(mergesToConsider);

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
        Set mergeableChildren;
        for (const auto& childID : mg.outNeigh.at(id)) {
            if (mg.mergeIsAcyclic(id, childID) && excludeSet.count(childID) == 0 && clustersToKeep.count(childID) == 0) {
                mergeableChildren.insert(childID);
            }
        }

        if (!mergeableChildren.empty()) {
            std::vector<NodeID> orderedByEdgesRemoved(mergeableChildren.begin(), mergeableChildren.end());
            // Seq tmpVec1({id, childID1});
            // Seq tmpVec2({id, childID2});
            //DEBUG 080723
            // std::sort(orderedByEdgesRemoved.begin(), orderedByEdgesRemoved.end(),
            //     [&](NodeID childID1, NodeID childID2) {
            //         std::vector<NodeID> tmpVec1 = {id, childID1};
            //         std::vector<NodeID> tmpVec2 = {id, childID2};
            //         return mg.numEdgesRemovedByMerge(tmpVec1) < mg.numEdgesRemovedByMerge(tmpVec2);
            //     });
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

    std::cout << "\nmergeSmallPartsDown_mergesToConsider\n";
    for(auto mergeGroup: mergesToConsider){
        for(auto m : mergeGroup){
            std::cout << m << "<2>";
        }
        std::cout << "<1>";
    }
    std::cout << "\n";

    //debug 080523
    std::cout << "mergeSmallPartsDown_mergesToConsider\n";
    for(auto mgroup : mergesToConsider){
        for(auto c : mgroup){
            std::cout << c << "<2>";
        }
        std::cout << "<1>";
    }
    std::cout << std::endl;

    //logger.info("  of " + std::to_string(smallPartIDs.size()) + " small parts " + std::to_string(mergesToConsider.size()) + " worth merging down");
    Seq mergesMade = performMergesIfPossible(mergesToConsider);

    if (!mergesMade.empty()) {
        mergeSmallPartsDown(smallPartCutoff);
    }
}

};

#endif