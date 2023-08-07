#ifndef GRAPH_H
#define GRAPH_H

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
#include <cassert>

#include <iostream>
#include <unordered_set>
#include <vector>
#include <functional>
#include <iostream>
#include <unordered_set>
#include <set>

#include <string>

using NodeID = int;

#define SHOW(X) std::cout << # X " = " << (X) << std::endl
#define PRINTLIST(vec) do { \
    std::cout << "Printing elements of the vector: "; \
    for (const auto& element : vec) { \
        std::cout << element << " "; \
    } \
    std::cout << std::endl; \
} while(0)

void testPrint( std::map<int, std::vector<int>> & m, int i )
{
  PRINTLIST( m[i] );
  SHOW( m.find(i)->first );
}

void checkMemberInMap(std::map<int, std::vector<int>>& input_map, int member) {
    bool isFound = false;

    for (const auto& val_pair : input_map) {
        if (val_pair.first == member) {
            isFound = true;
            break;
        }
    }

    std::cout << std::boolalpha << isFound << std::endl;
}

class graphInfo{
    public:
    std::vector<std::vector<int>> inNeigh;
    std::vector<std::vector<int>> outNeigh;
    std::map<int, std::vector<int>> mergeIDToMembers;
    std::vector<int> idToMergeID;
    std::vector<int> validNodes;
    std::vector<int> combinedNeighs();
    graphInfo(int nodeCount, int validCount){
        inNeigh.resize(nodeCount, std::vector<int>());
        outNeigh.resize(nodeCount, std::vector<int>());
        validNodes.resize(validCount);
    }
    graphInfo(){

    }
    void addEdge(int src, int dest){
        inNeigh[dest].push_back(src);
        outNeigh[src].push_back(dest);
    }
    void outputMGMergeInfo_modfile(std::string append){
        std::cout << "\n----------f?----------\n";
        std::cout << append << "\n\n";
        std::ofstream myfile;
        myfile.open("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/080623_mergeSmallPartsDown_merges_cpp",
            std::ios_base::app);
        myfile << "mergeIDToMembers\n";
        for( const auto &p : mergeIDToMembers){
        myfile << p.first;
        myfile << "<2>";
        for(const auto &member : p.second){
            myfile << member;
            myfile << "<3>";
        }
        myfile << "<1>";
        }
        myfile << "\n";
        myfile << "idToMergeID\n";
        for(const auto &mergeID : idToMergeID){
        myfile << mergeID;
        myfile << "<1>";
        }
        myfile << "\n";
        myfile.close();
    }
    void outputGNeighbors_modfile(std::string append){
        std::cout << "\n----------f?----------\n";
        std::cout << append << "\n\n";
        std::ofstream myfile;
        myfile.open("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/080623_mergeSmallPartsDown_neighs_cpp",
            std::ios_base::app);
        myfile << "inNeigh\n";
        for(int i=0; i<inNeigh.size(); i++){
            for(const auto &member : inNeigh[i]){
                myfile << member;
                myfile << "<2>";
            }
            myfile << "<1>";
        }
        myfile << "\n";
        myfile << "outNeigh\n";
        for(int i=0; i<outNeigh.size(); i++){
            for(const auto &member : outNeigh[i]){
                myfile << member;
                myfile << "<2>";
            }
            myfile << "<1>";
        }
        myfile << "\n";
        myfile.close();
    }
    //-------------------------------------------------------
    //helper for mergeNodesMutably by chatGPT
    std::vector<int> combineInNeigh( std::vector<int>& idsToMerge,  std::vector<std::vector<int>>& IONeigh, int mergeID) {
        std::set<int> combinedInNeigh;
        
        //080523 bug?
        // // for (const auto& id : idsToMerge) {
        // //     combinedInNeigh.insert(combinedInNeigh.end(), inNeigh[id].begin(), inNeigh[id].end());
        // // }
        // //BUG 071723: need to diff w/ mergeID
        // for (const auto& id : idsToMerge) {
        //     for (const auto& value : IONeigh[id]) {
        //         if (value != mergeID) {
        //             combinedInNeigh.push_back(value);
        //         }
        //     }
        // }
        
        // std::sort(combinedInNeigh.begin(), combinedInNeigh.end());
        // auto combinedInNeighEnd = std::unique(combinedInNeigh.begin(), combinedInNeigh.end());
        
        // std::vector<int> diff;
        // std::set_difference(combinedInNeigh.begin(), combinedInNeighEnd, idsToMerge.begin(), idsToMerge.end(),
        //                     std::back_inserter(diff));
        
        // return diff;

        for (const auto& id : idsToMerge) {
            for (const auto& value : IONeigh[id]) {
                if (value != mergeID && (std::find(idsToMerge.begin(), idsToMerge.end(), value) == idsToMerge.end())) {
                    combinedInNeigh.insert(value);
                }
            }
        }
        std::vector<int> combinedInNeighVec(combinedInNeigh.begin(), combinedInNeigh.end());
        std::sort(combinedInNeighVec.begin(), combinedInNeighVec.end());
        return combinedInNeighVec;
    }
    void mergeNeigh(NodeID outNeighID, const std::vector<NodeID>& idsToRemove, NodeID mergedID, std::vector<std::vector<int>>& IONeigh) {
        auto& neigh = IONeigh[outNeighID];
        // for(auto i : idsToRemove){
        //     std::cout << i;
        // }
        neigh.erase(std::remove_if(neigh.begin(), neigh.end(), [&](NodeID id) {
            return std::find(idsToRemove.begin(), idsToRemove.end(), id) != idsToRemove.end();
        }), neigh.end());
        
        if (std::find(neigh.begin(), neigh.end(), mergedID) == neigh.end()) {
            neigh.push_back(mergedID);
        }
    }

    std::map<int, std::vector<int>> groupIndicesByValue(const std::vector<int>& a) {
        std::map<int, std::vector<int>> result;
        for (int i = 0; i < a.size(); i++) {
            result[a[i]].push_back(i);
        }
        return result;
    }
    //-------------------------------------------------------

    void mergeNodesMutably(int mergeDest, std::vector<int>mergeSources) {
        int mergedID = mergeDest;
        std::vector<int> idsToRemove = mergeSources;
        std::vector<int> idsToMerge = mergeSources;
        idsToMerge.push_back(mergeDest);
        //std::vector<int> combinedInNeigh = idsToMerge.flatMap(inNeigh).distinct diff idsToMerge
        std::vector<int> combinedInNeigh = combineInNeigh(idsToMerge, inNeigh, mergeDest);
        std::vector<int> combinedOutNeigh = combineInNeigh(idsToMerge, outNeigh, mergeDest);
        // TODO: reduce redundancy with AddEdgeIfNew
        // combinedInNeigh foreach { inNeighID => {
        // outNeigh(inNeighID) --= idsToRemove
        // if (!outNeigh(inNeighID).contains(mergedID)) outNeigh(inNeighID) += mergedID
        // }}
        for (const auto& outNeighID : combinedOutNeigh) {
            mergeNeigh(outNeighID, idsToRemove, mergedID, inNeigh);
        }
        for (const auto& inNeighID : combinedInNeigh) {
            mergeNeigh(inNeighID, idsToRemove, mergedID, outNeigh);
        }
        //
        inNeigh[mergedID] = combinedInNeigh;
        outNeigh[mergedID] = combinedOutNeigh;
        // idsToRemove foreach { deleteID => {
        // inNeigh(deleteID).clear()
        // outNeigh(deleteID).clear()
        // }}
        for(auto deleteID : idsToRemove){
            inNeigh[deleteID].clear();
            outNeigh[deleteID].clear();
        }
    }
    void applyInitialAssignments(std::vector<int> initialAssignments) {
        // FUTURE: support negative (unassigned) initial assignments
        idToMergeID.clear();
        mergeIDToMembers.clear();
        idToMergeID = initialAssignments;
        std::map<int, std::vector<int>> asMap = groupIndicesByValue(initialAssignments);
        //generated chatgpt
        // Iterate over the mergeIDToMembers asMap
        for (const auto& pair : asMap) {
            NodeID mergeID = pair.first;
            const std::vector<NodeID>& members = pair.second;

            // Assert that members contains mergeID
            assert(std::find(members.begin(), members.end(), mergeID) != members.end());

            // Update mergeIDToMembers
            mergeIDToMembers[mergeID] = members;

            // Merge nodes mutably
            std::vector<NodeID> sources = members;
            sources.erase(std::remove(sources.begin(), sources.end(), mergeID), sources.end());
            std::sort(sources.begin(), sources.end(), [](int a, int b) { return a > b; });
            mergeNodesMutably(mergeID, sources);
        }
    }
    // void mergeGroups(NodeID mergeDest, std::vector<NodeID> mergeSources) {
    //     //val newMembers = (mergeSources map mergeIDToMembers).flatten
    //     std::vector<NodeID> newMembers;
    //     for (const auto& id : mergeSources) {
    //         const auto& members = mergeIDToMembers[id];
    //         newMembers.insert(newMembers.end(), members.begin(), members.end());
    //     }
    //     //newMembers foreach { id => idToMergeID(id) = mergeDest}
    //     for (const auto& id : newMembers) {
    //         idToMergeID[id] = mergeDest;
    //     }        
    //     //mergeIDToMembers(mergeDest) ++= newMembers
    //     mergeIDToMembers[mergeDest].insert(
    //         mergeIDToMembers[mergeDest].end(),
    //         newMembers.begin(),
    //         newMembers.end()
    //     );
    //     //mergeSources foreach { id => mergeIDToMembers.remove(id) }
    //     for (const auto& id : mergeSources) {
    //         mergeIDToMembers.erase(id);
    //     }
    //     mergeNodesMutably(mergeDest, mergeSources);
    // }
    int nodeSize(NodeID n){
        return mergeIDToMembers[n].size();
    }
    std::vector<int> getExcluded(){
        std::vector<int> total;
        std::vector<int> validFilter;
        total.resize(inNeigh.size() - validNodes.size() + 1, 0);
        //make binary filter to check if node is valid
        validFilter.resize(inNeigh.size(), 0);
        for(int i=0; i<validNodes.size(); i++){
            validFilter[validNodes[i]] = 1;
        }
        //iter over indicies, get only non-valid
        int idx = 0;
        for(int validIdx=0; validIdx<inNeigh.size(); validIdx++){
            if(validFilter[validIdx] == 0){
                total[idx] = validIdx;
                idx++;
            }
        }
        return total;
    }
    void mergeGroups(int mergeDest, const std::vector<int>& mergeSources) {
        std::vector<int> newMembers;
        for (int id : mergeSources) {
            const std::vector<int>& members = mergeIDToMembers[id];
            newMembers.insert(newMembers.end(), members.begin(), members.end());
            
            idToMergeID[id] = mergeDest;
        }
        
        std::vector<int>& destMembers = mergeIDToMembers[mergeDest];
        destMembers.insert(destMembers.end(), newMembers.begin(), newMembers.end());
        
        for (int id : mergeSources) {
            mergeIDToMembers.erase(id);
        }
        
        mergeNodesMutably(mergeDest, mergeSources);
    }


    //--------------------------------------------------------------------------------

    // typedef int NodeID;
    // typedef std::unordered_set<NodeID> std::unordered_set<int>;

    typedef int NodeID;
    typedef std::unordered_set<NodeID> Set;
    typedef std::unordered_map<NodeID, Set> NeighborsDict;

    bool extPathExists(NodeID source, NodeID dest) {
        return extPathExists(Set{source}, Set{dest});
    }

    bool traverseUntilIntersect(const Set& frontier, const Set reached, const Set& destSet) {
        if (frontier.empty()) {
            return false;
        } else {
            Set nextFrontier;
            for (const auto& node : frontier) {
                if(node > outNeigh.size()){
                    std::cout << "break?";
                }
                const auto& neighbors = outNeigh[node];
                for (const auto& neigh : neighbors) {
                    if (reached.find(neigh) == reached.end()) {
                        nextFrontier.insert(neigh);
                    }
                }
            }
            Set intersection;
            for (const auto& node : nextFrontier) {
                if (destSet.find(node) != destSet.end()) {
                    intersection.insert(node);
                }
            }
            if (!intersection.empty()) {
                return true;
            } else {
                Set nextReached;
                nextReached.insert(reached.begin(), reached.end());
                nextReached.insert(nextFrontier.begin(), nextFrontier.end());
                return traverseUntilIntersect(nextFrontier, nextReached, destSet);
            }
        }
    };

    bool extPathExists(const Set& sourceSet, const Set& destSet) {
        Set sourcesOnFringe;
        for (const auto& id : sourceSet) {
            bool exists = false;
            const auto& neighbors = outNeigh[id];
            for (const auto& neigh : neighbors) {
                if (sourceSet.find(neigh) == sourceSet.end()) {
                    exists = true;
                    break;
                }
            }
            if (exists) {
                sourcesOnFringe.insert(id);
            }
        }
        Set startingExtFrontier;
        for (const auto& source : sourcesOnFringe) {
            const auto& neighbors = outNeigh[source];
            for (const auto& neigh : neighbors) {
                if (sourceSet.find(neigh) == sourceSet.end() && destSet.find(neigh) == destSet.end()) {
                    startingExtFrontier.insert(neigh);
                }
            }
        }


        return traverseUntilIntersect(startingExtFrontier, sourceSet, destSet);
    }

    bool mergeIsAcyclic(NodeID u, NodeID v) {
        return !extPathExists(u, v) && !extPathExists(v, u);
    }

    bool mergeIsAcyclic(const std::unordered_set<NodeID>& ids) {
        for (const auto& source : ids) {
            Set remainingIds = ids;
            remainingIds.erase(source);
            if (extPathExists({source}, remainingIds)) {
                return false;
            }
        }
        return true;
    }

    int numEdgesRemovedByMerge(const std::vector<NodeID>& mergeReq) {
        int totalInDegree = 0;
        int totalOutDegree = 0;
        int mergedInDegree = 0;
        int mergedOutDegree = 0;
        Set mergedInDegreeSet;
        Set mergedOutDegreeSet;
        std::vector<NodeID> mergeReqTmp(mergeReq.begin(), mergeReq.end());
        for (const auto& id : mergeReq) {
            totalInDegree += inNeigh.at(id).size();
            totalOutDegree += outNeigh.at(id).size();

            // Set mergedInDegreeSet;
            //for (const auto& inNeighbor : inNeigh.at(id)) {
            for (const auto& inNeighbor : inNeigh[id]) {    
                if (std::find(mergeReqTmp.begin(), mergeReqTmp.end(), inNeighbor) == mergeReqTmp.end()) {
                    mergedInDegreeSet.insert(inNeighbor);
                }
            }

            // Set mergedOutDegreeSet;
            //for (const auto& outNeighbor : outNeigh.at(id)) {
            for (const auto& outNeighbor : outNeigh[id]) {
                if (std::find(mergeReqTmp.begin(), mergeReqTmp.end(), outNeighbor) == mergeReqTmp.end()) {
                    mergedOutDegreeSet.insert(outNeighbor);
                }
            }
            // mergedInDegreeSet.clear();
            // mergedOutDegreeSet.clear();
        }
        mergedInDegree += mergedInDegreeSet.size();
        mergedOutDegree += mergedOutDegreeSet.size();
            
        return totalInDegree + totalOutDegree - (mergedInDegree + mergedOutDegree);
    }

};

#endif