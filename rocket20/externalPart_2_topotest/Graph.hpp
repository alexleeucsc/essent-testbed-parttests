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
#include <ctime>

#include "Index.h"
#include <bits/stdc++.h>
#include <deque>

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

bool checkMemberInMap(std::map<int, std::vector<int>>& input_map, int member) {
    bool isFound = false;

    for (const auto& val_pair : input_map) {
        if (val_pair.first == member) {
            isFound = true;
            break;
        }
    }

    std::cout << std::boolalpha << isFound << std::endl;
    return isFound;
}

void checkMemberInMapVals(std::map<int, std::vector<int>>& input_map, int member) {
    bool isFound = false;

    for (const auto& val_pair : input_map) {
        for (const auto& vals : val_pair.second) {
            if (vals == member) {
                break;
            }
        }
    }

    std::cout << "member is in values"<< std::endl;
}

int searchGraphlike(std::vector<std::vector<int>>& input_map, int member) {
    
    std::deque<int> frontier{member};
    std::vector<bool> visited(input_map.size()+10, false);
    std::deque<int> newFrontier;
    int distFromStart = 0;
    bool firstIter = true;
    while(frontier.size() > 0 || firstIter){
        firstIter = false;
        distFromStart++;
        while(frontier.size() > 0){
            int toProc = frontier.front();
            frontier.pop_front();
            for(auto neigh : input_map[toProc]){
                if(!visited[neigh]){
                    newFrontier.push_back(neigh);
                    visited[neigh] = true;
                }
            }
        }
        frontier = newFrontier;
        std::cout << "distFromStart=" << distFromStart << ": ";
        for(auto f : frontier){
            std::cout << f << ", ";
        }
        std::cout <<"\n";
        newFrontier.clear();
    }

    std::cout << "member is in values"<< std::endl;
    return distFromStart;
}


int dbgfilter1_searchGraphlike(std::vector<std::vector<int>>& input_map, int member, std::vector<int>& dists, int expDistFilter) {
    
    std::deque<int> frontier{member};
    std::vector<bool> visited(input_map.size()+10, false);
    std::deque<int> newFrontier;
    int distFromStart = 0;
    bool firstIter = true;
    while(frontier.size() > 0 || firstIter){
        firstIter = false;
        distFromStart++;
        expDistFilter--;
        while(frontier.size() > 0){
            int toProc = frontier.front();
            frontier.pop_front();
            for(auto neigh : input_map[toProc]){
                if(!visited[neigh]){
                    newFrontier.push_back(neigh);
                    visited[neigh] = true;
                }
            }
        }
        frontier = newFrontier;
        std::cout << "distFromStart=" << distFromStart << ": ";
        for(auto f : frontier){
            if(dists[f] == expDistFilter){
                std::cout << f << ", ";
            }
        }
        std::cout <<"\n";
        newFrontier.clear();
    }

    std::cout << "member is in values"<< std::endl;
    return distFromStart;
}


bool searchGraphlike(std::vector<std::vector<int>>& input_map, int member, int target) {
    
    std::deque<int> frontier{member};
    std::vector<bool> visited(input_map.size()+10, false);
    std::deque<int> newFrontier;
    int distFromStart = 0;
    bool firstIter = true;
    while(frontier.size() > 0 || firstIter){
        firstIter = false;
        distFromStart++;
        while(frontier.size() > 0){
            int toProc = frontier.front();
            frontier.pop_front();
            for(auto neigh : input_map[toProc]){
                if(neigh == target){
                    std::cout << "found!\n";
                    return true;
                }
                if(!visited[neigh]){
                    newFrontier.push_back(neigh);
                    visited[neigh] = true;
                }
            }
        }
        frontier = newFrontier;
        std::cout << "distFromStart=" << distFromStart << ": ";
        for(auto f : frontier){
            std::cout << f << ", ";
        }
        std::cout <<"\n";
        newFrontier.clear();
    }

    std::cout << "member is in values"<< std::endl;
    return false;
}

bool checkIllegalMemberInMapVals(std::map<int, std::vector<int>>& input_map) {
    bool isFound = false;
    int illegalVal = 0;
    std::vector<int> illegalVals;
    for (const auto& val_pair : input_map) {
        for (const auto& vals : val_pair.second) {
            if (input_map.find(vals) != input_map.end() && vals != val_pair.first) {
                illegalVal = vals;
                isFound = true;
                illegalVals.push_back(vals);
            }
        }
    }
    if(isFound){
        std::cout << "illegal members: ";
        for(auto iVal: illegalVals){std::cout<<iVal<<", ";}
        std::cout<<std::endl;
    }
    return isFound;
}

void checkMemberInVector(const std::vector<int>& vec, int member) {
    bool isFound = false;

    for (int i=0; i<vec.size(); i++) {
        int element = vec[i];
        if (element == member) {
            isFound = true;
            std::cout << isFound << " : " << i << std::endl;
        }
    }
    if(!isFound){
        std::cout << std::boolalpha << isFound << std::endl;
    }
}

class graphInfo{
    public:
    std::vector<std::vector<int>> inNeigh;
    std::vector<std::vector<int>> outNeigh;
    std::map<int, std::vector<int>> mergeIDToMembers;
    std::vector<int> idToMergeID;
    std::vector<int> validNodes;

    //DEBUG FLAGS
    int debugTimingPerSearch = 0;
    int debugTraversalLists = 0;

    //std::vector<int> serachedCount_profile;             //082523 debug
    int longSearchThresh = 100;
    std::vector<int> combinedNeighs();
    graphInfo(int nodeCount, int validCount){
        inNeigh.resize(nodeCount, std::vector<int>());
        outNeigh.resize(nodeCount, std::vector<int>());
        validNodes.resize(validCount);
        //serachedCount_profile.resize(nodeCount+1, 0);        //082523 debug
    }
    graphInfo(){

    }
    void addEdge(int src, int dest){
        inNeigh[dest].push_back(src);
        outNeigh[src].push_back(dest);
    }
    void outputMGMergeInfo_modfile(std::string append){
        //std::cout << "\n----------f?----------\n";
        //std::cout << append << "\n\n";
        std::ofstream myfile;
        myfile.open("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart_2_topotest/example.txt",
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
        //std::cout << "\n----------f?----------\n";
        //std::cout << append << "\n\n";
        std::ofstream myfile;
        myfile.open("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart_2_topotest/example.txt",
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
        //     //std::cout << i;
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
        // if(checkIllegalMemberInMapVals(mergeIDToMembers)){
        //     std::cout<<"illegal node found\n";
        // }
    }


    //--------------------------------------------------------------------------------

    // typedef int NodeID;
    // typedef std::unordered_set<NodeID> std::unordered_set<int>;

    typedef int NodeID;
    typedef std::set<NodeID> Set;
    typedef std::unordered_map<NodeID, Set> NeighborsDict;

    bool extPathExists(NodeID source, NodeID dest) {
        Set sourceSet{source};
        return extPathExists(sourceSet, Set{dest});
    }

    bool extPathExists_Indexed(Set& sourceSet, const Set& destSet, Index& bm) {
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
        bool reachable1 = bm.reachable(*startingExtFrontier.begin(),*destSet.begin());
        //bool reachable2 = bm.reachable(*destSet.begin(), *sourceSet.begin());
        if(reachable1){
            if(sourceSet.size()!=1 || destSet.size()!=1){
                std::cout<<"unexpected size\n";
            }
            bool reachableTrue = traverseUntilIntersect(startingExtFrontier, sourceSet, destSet, 0);
            // if(reachableTrue != reachable1){
            //     std::cout << "problem! index says nodes are reachable, but bfs says they are not!\n";
            //     std::cout<<*startingExtFrontier.begin()<<", "<<*destSet.begin()<<"\n";
            // }
            return true;
        }else{
            return traverseUntilIntersect(startingExtFrontier, sourceSet, destSet, 0);
        }
    }
    bool extPathExists_LayerIndexed(Set& sourceSet, const Set& destSet, std::vector<int>& dists) {
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
        int cutoff = -1;
        std::cout << "\t\tdist=";
        for(auto s : destSet){
            if(dists[s] > cutoff){
                std::cout << dists[s] << " - ";
                cutoff = dists[s];
            }
        }
        std::cout << "\n";
        return traverseUntilIntersect_LayerIndexed(startingExtFrontier, sourceSet, destSet, 0, dists, cutoff);
    }

    std::tuple<bool,bool> traverseUntilIntersectLimit(const Set& frontier, const Set reached, const Set& destSet, const int nodesSearched) {
        int nodesSearchedIter = nodesSearched; 
        if (frontier.empty()) {
            // if(debugTimingPerSearch){
            //     std::cout << "\nsearched:" << nodesSearchedIter << "\t" << "false" << std::endl;
            // }
            return std::tuple<bool,int>(false,false);               //082823: if there are no more nodes to search, we did not time out
        } else {
            Set nextFrontier;
            for (const auto& node : frontier) {
                if(node > outNeigh.size()){
                    //std::cout << "break?";
                }
                const auto& neighbors = outNeigh[node];
                for (const auto& neigh : neighbors) {
                    nodesSearchedIter += 1;
                    //serachedCount_profile[neigh] += 1;              //082523 debug
                    // if(debugTimingPerSearch){
                    //     std::cout << neigh << ", ";                     //082823 debug
                    // }
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
                // if(debugTimingPerSearch){
                //     std::cout << "\nsearched:" << nodesSearchedIter << "\t" << "true" << std::endl;
                // }
                return  std::tuple<bool,bool>(true,false);          //082823 : if we found a path, then we did not time out
            } else {
                Set nextReached;
                nextReached.insert(reached.begin(), reached.end());
                nextReached.insert(nextFrontier.begin(), nextFrontier.end());
                if (nodesSearchedIter <= longSearchThresh){
                    return traverseUntilIntersectLimit(nextFrontier, nextReached, destSet, nodesSearchedIter);
                } else{
                    return  std::tuple<bool,bool>(false,true);  
                }
            }
        }
    };
    bool traverseUntilIntersect(const Set& frontier, Set& reached, const Set& destSet, const int nodesSearched) {
        //int nodesSearchedIter = nodesSearched; 
        if (frontier.empty()) {
            // if(debugTimingPerSearch){
            //     std::cout << "\nsearched:" << nodesSearchedIter << "\t" << "false" << std::endl;
            // }
            return false;
        } else {
            Set nextFrontier;
            for (const auto& node : frontier) {
                if(node > outNeigh.size()){
                    //std::cout << "break?";
                }
                const auto& neighbors = outNeigh[node];
                //if(neighbors.size() < reached.size()){
                    for (const auto& neigh : neighbors) {
                        //nodesSearchedIter += 1;
                        //serachedCount_profile[neigh] += 1;              //082523 debug
                        // if(debugTimingPerSearch){
                        //     std::cout << neigh << ", ";                     //082823 debug
                        // }
                        if (reached.find(neigh) == reached.end()) {
                            nextFrontier.insert(neigh);
                        }
                    }
                // }else{
                //     Set neighborsSet(neighbors.begin(), neighbors.end());
                //     for (const auto& reachNode : reached) {
                //         //nodesSearchedIter += 1;
                //         //serachedCount_profile[neigh] += 1;              //082523 debug
                //         // if(debugTimingPerSearch){
                //         //     std::cout << neigh << ", ";                     //082823 debug
                //         // }
                //         if (neighborsSet.find(reachNode) == reached.end()) {
                //             nextFrontier.insert(reachNode);
                //         }
                //     }  
                // }
            }
            //Set intersection;
            bool intersectEmpty = true;
            // for (const auto& node : nextFrontier) {
            //     if (destSet.find(node) != destSet.end()) { //bitmap here?
            //         //intersection.insert(node);
            //         intersectEmpty = false;
            //         break;
            //     }
            // }
            for (const auto& node : destSet) {
                if (nextFrontier.find(node) != nextFrontier.end()) { //bitmap here?
                    //intersection.insert(node);
                    intersectEmpty = false;
                    break;
                }
            }
            //if (!intersection.empty()) {
            if (!intersectEmpty) {
                // if(debugTimingPerSearch){
                //     std::cout << "\nsearched:" << nodesSearchedIter << "\t" << "true" << std::endl;
                // }
                return true;                                        //082823 : if we found a path, then we did not time out
            } else {
                // Set nextReached;
                // nextReached.insert(reached.begin(), reached.end());
                // nextReached.insert(nextFrontier.begin(), nextFrontier.end());
                reached.insert(nextFrontier.begin(), nextFrontier.end());
                return traverseUntilIntersect(nextFrontier, reached, destSet, 0);
            }
        }
    };
    bool traverseUntilIntersectMemo( Set& frontier,  Set reached, const Set& destSet, const int nodesSearched) {
        //int nodesSearchedIter = nodesSearched;
        while(!frontier.empty()){
            Set nextFrontier;
            for (const auto& node : frontier) {
                if(node > outNeigh.size()){
                    //std::cout << "break?";
                }
                const auto& neighbors = outNeigh[node];
                for (const auto& neigh : neighbors) {
                    //nodesSearchedIter += 1;
                    //serachedCount_profile[neigh] += 1;              //082523 debug
                    // if(debugTimingPerSearch){
                    //     std::cout << neigh << ", ";                     //082823 debug
                    // }
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
                // if(debugTimingPerSearch){
                //     std::cout << "\nsearched:" << nodesSearchedIter << "\t" << "true" << std::endl;
                // }
                return true;                                        //082823 : if we found a path, then we did not time out
            } else {
                Set nextReached;
                nextReached.insert(reached.begin(), reached.end());
                nextReached.insert(nextFrontier.begin(), nextFrontier.end());
                frontier = nextFrontier;
                reached = nextReached;
            }
        }
        // if(debugTimingPerSearch){
        //     std::cout << "\nsearched:" << nodesSearchedIter << "\t" << "false" << std::endl;
        // }
        return false;
    };
    bool traverseUntilIntersect_LayerIndexed(const Set& frontier, Set& reached, const Set& destSet, const int nodesSearched, std::vector<int>& dists, int cutoff) {
        //int nodesSearchedIter = nodesSearched; 
        if (frontier.empty()) {
            // if(debugTimingPerSearch){
            //     std::cout << "\nsearched:" << nodesSearchedIter << "\t" << "false" << std::endl;
            // }
            return false;
        } else {
            Set nextFrontier;
            for (const auto& node : frontier) {
                if(node > outNeigh.size()){
                    //std::cout << "break?";
                }
                const auto& neighbors = outNeigh[node];
                //if(neighbors.size() < reached.size()){
                    for (const auto& neigh : neighbors) {
                        //nodesSearchedIter += 1;
                        //serachedCount_profile[neigh] += 1;              //082523 debug
                        // if(debugTimingPerSearch){
                        //     std::cout << neigh << ", ";                     //082823 debug
                        // }
                        if (dists[neigh] <= cutoff && reached.find(neigh) == reached.end()) {
                            nextFrontier.insert(neigh);
                        }
                    }
                // }else{
                //     Set neighborsSet(neighbors.begin(), neighbors.end());
                //     for (const auto& reachNode : reached) {
                //         //nodesSearchedIter += 1;
                //         //serachedCount_profile[neigh] += 1;              //082523 debug
                //         // if(debugTimingPerSearch){
                //         //     std::cout << neigh << ", ";                     //082823 debug
                //         // }
                //         if (neighborsSet.find(reachNode) == reached.end()) {
                //             nextFrontier.insert(reachNode);
                //         }
                //     }  
                // }
            }
            //Set intersection;
            bool intersectEmpty = true;
            // for (const auto& node : nextFrontier) {
            //     if (destSet.find(node) != destSet.end()) { //bitmap here?
            //         //intersection.insert(node);
            //         intersectEmpty = false;
            //         break;
            //     }
            // }
            for (const auto& node : destSet) {
                if (nextFrontier.find(node) != nextFrontier.end()) { //bitmap here?
                    //intersection.insert(node);
                    intersectEmpty = false;
                    break;
                }
            }
            //if (!intersection.empty()) {
            if (!intersectEmpty) {
                // if(debugTimingPerSearch){
                //     std::cout << "\nsearched:" << nodesSearchedIter << "\t" << "true" << std::endl;
                // }
                return true;                                        //082823 : if we found a path, then we did not time out
            } else {
                // Set nextReached;
                // nextReached.insert(reached.begin(), reached.end());
                // nextReached.insert(nextFrontier.begin(), nextFrontier.end());
                reached.insert(nextFrontier.begin(), nextFrontier.end());
                return traverseUntilIntersect_LayerIndexed(nextFrontier, reached, destSet, 0, dists, cutoff);
            }
        }
    };

    std::tuple<bool,bool> extPathExistsLimited(const Set& sourceSet, const Set& destSet) {
    int begin_extPathExists = std::clock();
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

        if(debugTraversalLists){
            std::cout << "traverseUntilIntersectLog\n";
        }
        std::tuple<bool,bool> out_limited = traverseUntilIntersectLimit(startingExtFrontier, sourceSet, destSet, 0);
        //out = true if there was an intersect (path exists) and flase if there was not an intersect (path did not exist)
        //limited = false if not limited (use out) and true if was limited (save for later)
        bool out = std::get<0>(out_limited);
        bool limited = std::get<1>(out_limited);
        if(debugTraversalLists){
            std::cout << "\n";
        }
    int end_extPathExists = std::clock();
    if(debugTimingPerSearch){
        std::cout << "time: " << end_extPathExists - begin_extPathExists << "\n";
    }
        return out_limited;
    }
    bool extPathExists(Set& sourceSet, const Set& destSet) {
    int begin_extPathExists = std::clock();
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
        if(debugTraversalLists){
            std::cout << "traverseUntilIntersectLog\n";
        }
        bool out = traverseUntilIntersect(startingExtFrontier, sourceSet, destSet, 0);
        if(debugTraversalLists){
            std::cout << "\n";
        }
    int end_extPathExists = std::clock();
    if(debugTimingPerSearch){
        std::cout << "time: " << end_extPathExists - begin_extPathExists << "\n";
    }
        return out;
    }
    bool extPathExistsPrintout(Set& sourceSet, const Set& destSet) {
    int begin_extPathExists = std::clock();
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
        if(debugTraversalLists){
            std::cout << "traverseUntilIntersectLog\n";
        }
    int mid_extPathExists = std::clock();
    std::cout << "MIAinit: " << mid_extPathExists - begin_extPathExists << "\n";
        bool out = traverseUntilIntersect(startingExtFrontier, sourceSet, destSet, 0);
        if(debugTraversalLists){
            std::cout << "\n";
        }
    int end_extPathExists = std::clock();
    std::cout << "MIAtrav: " << end_extPathExists - mid_extPathExists << "\n";
        return out;
    }
    bool extPathExistsMemo(const Set& sourceSet, const Set& destSet, const std::vector<int>& visitedfrontier) {
    int begin_extPathExists = std::clock();
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
        if(debugTraversalLists){
            std::cout << "traverseUntilIntersectMemoLog\n";
        }
        bool out = traverseUntilIntersectMemo(startingExtFrontier, sourceSet, destSet, 0);
        if(debugTraversalLists){
            std::cout << "\n";
        }
    int end_extPathExists = std::clock();
    if(debugTimingPerSearch){
        std::cout << "time: " << end_extPathExists - begin_extPathExists << "\n";
    }
        return out;
    }

    bool mergeIsAcyclic(NodeID u, NodeID v) {
        return !extPathExists(u, v) && !extPathExists(v, u);
    }

    bool mergeIsAcyclic(const std::set<NodeID>& ids) {
        for (const auto& source : ids) {
            // if(debugTimingPerSearch){
            //     std::cout << "src:" << source << std::endl;
            // }
            Set remainingIds = ids;
            remainingIds.erase(source);
            Set sourceSet{source};
            bool out = extPathExists(sourceSet, remainingIds);
            if (out) {
                return false;
            }
        }
        return true;
    }
    bool mergeIsAcyclicPrintout(const std::set<NodeID>& ids) {
        clock_t begin = clock();
        for (const auto& source : ids) {
            // if(debugTimingPerSearch){
            //     std::cout << "src:" << source << std::endl;
            // }
            Set remainingIds = ids;
            remainingIds.erase(source);
            Set sourceSet{source};
            bool out = extPathExistsPrintout(sourceSet, remainingIds);
            if (out) {
                return false;
            }
        }
        clock_t end = clock();
        double elapsed_secs = double(end - begin);
        std::cout << "\t\tmergeIsAcyclic " << ids.size() << " :" << elapsed_secs << "\n";
        return true;
    }
    std::tuple<bool,bool> mergeIsAcyclicLimited(const std::set<NodeID>& ids) {
        for (const auto& source : ids) {
            // if(debugTimingPerSearch){
            //     std::cout << "src:" << source << std::endl;
            // }
            Set remainingIds = ids;
            remainingIds.erase(source);
            std::tuple<bool,bool> out_limited = extPathExistsLimited({source}, remainingIds);
            bool out = std::get<0>(out_limited);
            bool limited = std::get<1>(out_limited);
            if (out) {
                return std::tuple<bool,bool>(false,false);
            }
            else if(limited){
                return std::tuple<bool,bool>(false,true);
            }
        }
        return std::tuple<bool,bool>(true,false);
    }
    bool mergeIsAcyclicMemo(const std::set<NodeID>& ids, const std::vector<int>& visited, const std::vector<int>& visitedfrontier) {
        for (const auto& source : ids) {
            if(debugTimingPerSearch){
                std::cout << "src:" << source << std::endl;
            }
            Set remainingIds = ids;
            remainingIds.erase(source);
            bool out = extPathExistsMemo({source}, remainingIds, visitedfrontier);
            if (out) {
                return false;
            }
        }
        return true;
    }
    bool mergeIsAcyclic_Indexed(const std::set<NodeID>& ids, Index& bm) {
        for (const auto& source : ids) {
            Set remainingIds = ids;
            remainingIds.erase(source);
            Set sourceSet{source};
            if (extPathExists_Indexed(sourceSet, remainingIds, bm)) {
                return false;
            }
        }
        return true;
    }
    bool mergeIsAcyclic_LayerIndexed(const std::set<NodeID>& ids, std::vector<int>& dists) {
        for (const auto& source : ids) {
            Set remainingIds = ids;
            remainingIds.erase(source);
            Set sourceSet{source};
            if (extPathExists_LayerIndexed(sourceSet, remainingIds, dists)) {
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
        std::set<NodeID> mergeReqTmp(mergeReq.begin(), mergeReq.end());
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

    void fixIndex(Set mergeReq, std::vector<int> & dists, int nameOfNewNode){
        //get max dist
        int maxDist = -1;
        int furthestNode = -1;
        for(auto m : mergeReq){
            if (dists[m] > maxDist){
                maxDist = dists[m];
                furthestNode = m;
            }
        }
        //iter over lesser dists
        mergeReq.erase(furthestNode);
        //assert that the longest path to furthestNode isn't only through the other node
        // if(){

        // }
        //do BFS over all lesser dists
        for(auto remain_m : mergeReq){
            std::vector<bool> visited(outNeigh.size()+10, false);
            std::deque<int> frontier{remain_m};
            std::deque<int> newFrontier;
            int distFromStart = 0;
            bool firstIter = true;
            while(newFrontier.size() > 0 || firstIter){
                firstIter = false;
                distFromStart++;
                while(frontier.size() > 0){
                    int toProc = frontier.front();
                    frontier.pop_front();
                    for(auto neigh : outNeigh[toProc]){
                        if(!visited[neigh] & maxDist + distFromStart > dists[neigh]){
                            newFrontier.push_back(neigh);
                            visited[neigh] = true;
                            dists[neigh] = maxDist + distFromStart;
                        }
                    }
                }
                frontier = newFrontier;
                newFrontier.clear();
            }
        }
        dists[nameOfNewNode] = maxDist;
        //increment the dists as needed 
    }

    //--------------------TOPO SORT FROM G$G--------------------
    // A recursive function used by topologicalSort
    void topologicalSortUtil(int v, std::vector<bool>& visited,
                                    std::vector<int>& Stack)
    {
        // if(isCyclic()){
        //     std::cout<<"break\n";
        // }
        // Mark the current node as visited.
        visited[v] = true;
    
        // Recur for all the vertices adjacent to this vertex
        std::vector<int>::iterator i;
        for (i = outNeigh[v].begin(); i != outNeigh[v].end(); ++i){
            if (!visited[*i]){
                //clock_t begin = clock();
                topologicalSortUtil(*i, visited, Stack);
                //clock_t end = clock();
                ///double elapsed_secs = double(end - begin);
                ////std::cout << "topologicalSortUtil inner call: " << elapsed_secs << "\n";
            }
            // cycle checking condition!
            // if(std::find(Stack.begin(),Stack.end(),v )!= Stack.end()){
            //     std::cout<<"prob\n";
            // }
        }
    
        // Push current vertex to stack which stores result
        Stack.push_back(v);
    }
    
    // The function to do Topological Sort. It uses recursive
    // topologicalSortUtil()
    std::vector<int> topologicalSort()
    {
        std::vector<int> Stack;
    
        // Mark all the vertices as not visited
        std::vector<bool> visited(outNeigh.size(), false);  //NOTE: should be fine to use outNeigh.size, but may casue bugs
    
        // Call the recursive helper function to store Topological
        // Sort starting from all vertices one by one
        for (int i = 0; i < visited.size(); i++){
            if (visited[i] == false){
                clock_t begin = clock();
                topologicalSortUtil(i, visited, Stack);
                clock_t end = clock();
                double elapsed_secs = double(end - begin);
                //std::cout << "topologicalSortUtil call: " << elapsed_secs << "\n";
            }
        }
    
        std::reverse(Stack.begin(), Stack.end());
        return Stack;
    }

    // The function to do Topological Sort. It uses recursive
    // topologicalSortUtil()
    std::tuple<std::vector<std::vector<int>>, std::vector<int>> layerList() {
        // Initialize distances to all vertices as minus infinity and distance to startVertex as 0
        std::vector<int> dist(outNeigh.size(), -1);
        std::vector<std::vector<int>> layerList;
        for (int n=0; n<outNeigh.size(); n++){
            if(inNeigh[n].size()==0){
                dist[n] = 0;
            }
        }

        // Get topological sort of all vertices
        std::vector<int> topoOrder = topologicalSort();  // Assuming you have this function
        
        // Find longest paths
        for (int i : topoOrder) {
            // Continue if dist[i] is still the initialized value (i.e., vertex i is not reachable from startVertex)
            if (dist[i] == -1) continue;

            // Update dist value of all adjacent vertices of i
            //if (outNeigh.find(i) != outNeigh.end()) {
                for (int neighbor : outNeigh[i]) {
                    if (dist[neighbor] < dist[i] + 1) {  // Here, edge weight is considered 1
                        dist[neighbor] = dist[i] + 1;
                    }
                }
            //}
            //if the longestDist already has a list, just emplace into layerList
            if(dist[i] >= layerList.size()){
                while(layerList.size() <= dist[i]){
                    std::vector<int> newLayerList;
                    layerList.push_back(newLayerList);
                }
            }
            if(dist[i] >= layerList.size()){
                std::cout << "break\n";
            }
            layerList[dist[i]].push_back(i);
        }

        return std::tuple<std::vector<std::vector<int>>, std::vector<int>>(layerList, dist);
    }


    bool isCyclicUtil(int v, bool visited[],
                            bool* recStack)
    {
        if (visited[v] == false) {
            // Mark the current node as visited
            // and part of recursion stack
            visited[v] = true;
            recStack[v] = true;
    
            // Recur for all the vertices adjacent to this
            // vertex
            std::vector<int>::iterator i;
            for (i = outNeigh[v].begin(); i != outNeigh[v].end(); ++i) {
                if (!visited[*i]
                    && isCyclicUtil(*i, visited, recStack))
                    return true;
                else if (recStack[*i])
                    return true;
            }
        }
    
        // Remove the vertex from recursion stack
        recStack[v] = false;
        return false;
    }
    bool isCyclic()
    {
        // Mark all the vertices as not visited
        // and not part of recursion stack
        bool* visited = new bool[outNeigh.size()];
        bool* recStack = new bool[outNeigh.size()];
        for (int i = 0; i < outNeigh.size(); i++) {
            visited[i] = false;
            recStack[i] = false;
        }
    
        // Call the recursive helper function
        // to detect cycle in different DFS trees
        for (int i = 0; i < outNeigh.size(); i++)
            if (!visited[i]
                && isCyclicUtil(i, visited, recStack))
                return true;
    
        return false;
    }

};

#endif