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
using NodeID = int;
using Graph = std::map<int, std::vector<int>>;

class GraphInfo{
    public:
    std::map<int, std::vector<int>> inNeigh;
    std::map<int, std::vector<int>> outNeigh;
    std::map<int, std::vector<int>> mergeIDToMembers;
    std::vector<int> idToMergeID;
    std::vector<int> validNodes;
    std::vector<int> combinedNeighs();
    //-----
    //helper for mergeNodesMutably by chatGPT
    std::vector<NodeID> combineInNeigh(std::vector<NodeID>& idsToMerge, Graph& inNeigh) {
        std::vector<NodeID> combinedInNeigh;
        
        for (const auto& id : idsToMerge) {
            combinedInNeigh.insert(combinedInNeigh.end(), inNeigh[id].begin(), inNeigh[id].end());
        }
        
        std::sort(combinedInNeigh.begin(), combinedInNeigh.end());
        auto combinedInNeighEnd = std::unique(combinedInNeigh.begin(), combinedInNeigh.end());
        
        std::vector<NodeID> diff;
        std::set_difference(combinedInNeigh.begin(), combinedInNeighEnd, idsToMerge.begin(), idsToMerge.end(),
                            std::back_inserter(diff));
        
        return diff;
    }
    void mergeOutNeigh(NodeID outNeighID, const std::vector<NodeID>& idsToRemove, NodeID mergedID, Graph& inNeigh) {
        auto& neigh = inNeigh[outNeighID];
        neigh.erase(std::remove_if(neigh.begin(), neigh.end(), [&](NodeID id) {
            return std::find(idsToRemove.begin(), idsToRemove.end(), id) != idsToRemove.end();
        }), neigh.end());
        
        if (std::find(neigh.begin(), neigh.end(), mergedID) == neigh.end()) {
            neigh.push_back(mergedID);
        }
    }    
    //-----
    void mergeNodesMutably(int mergeDest, std::vector<int>mergeSources) {
        int mergedID = mergeDest;
        std::vector<int> idsToRemove = mergeSources;
        std::vector<int> idsToMerge = mergeSources;
        idsToMerge.push_back(mergeDest);
        //std::vector<int> combinedInNeigh = idsToMerge.flatMap(inNeigh).distinct diff idsToMerge
        std::vector<int> combinedInNeigh = combineInNeigh(idsToMerge, inNeigh);
        std::vector<int> combinedOutNeigh = combineInNeigh(idsToMerge, outNeigh);
        // TODO: reduce redundancy with AddEdgeIfNew
        // combinedInNeigh foreach { inNeighID => {
        // outNeigh(inNeighID) --= idsToRemove
        // if (!outNeigh(inNeighID).contains(mergedID)) outNeigh(inNeighID) += mergedID
        // }}
        for (const auto& outNeighID : combinedOutNeigh) {
            mergeOutNeigh(outNeighID, idsToRemove, mergedID, inNeigh);
        }
        for (const auto& inNeighID : combinedInNeigh) {
            mergeOutNeigh(inNeighID, idsToRemove, mergedID, outNeigh);
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
};



// Test harness
void runTest(const std::string& testName, bool result) {
    std::cout << testName << ": " << (result ? "Passed" : "Failed") << std::endl;
}

// Test case for mergeNodesMutably function
void testMergeNodesMutably() {
    GraphInfo graphInfo;

    // Construct the directed graph
    graphInfo.inNeigh = {
        {1, {}},            // Node 1 has no source neighbors
        {2, {1}},           // Node 2 has node 1 as the source neighbor
        {3, {2}},           // Node 3 has node 2 as the source neighbor
        {4, {1, 3}},        // Node 4 has nodes 1 and 3 as source neighbors
        {5, {4}},           // Node 5 has node 4 as the source neighbor
    };

    graphInfo.outNeigh = {
        {1, {2, 4}},        // Node 1 has nodes 2 and 4 as destination neighbors
        {2, {3}},           // Node 2 has node 3 as the destination neighbor
        {3, {4}},           // Node 3 has node 4 as the destination neighbor
        {4, {5}},           // Node 4 has node 5 as the destination neighbor
        {5, {}}             // Node 5 has no destination neighbors
    };

    // Test merging nodes with correct conditions
    graphInfo.mergeNodesMutably(6, {1, 3});
    bool testResult1 = (graphInfo.inNeigh[2] == std::vector<int>{6});      // Node 2 should have updated source neighbor
    bool testResult2 = (graphInfo.outNeigh[2] == std::vector<int>{6});     // Node 2 should have updated destination neighbor
    bool testResult3 = (graphInfo.inNeigh[4] == std::vector<int>{6});      // Node 4 should have updated source neighbor
    bool testResult4 = (graphInfo.outNeigh[4] == std::vector<int>{5});     // Node 3 should have updated destination neighbor
    bool testResult5 = (graphInfo.inNeigh[5] == std::vector<int>{4});  // Node 6 should have updated destination neighbors
    runTest("Merge Nodes Test - Correct Conditions", testResult1 && testResult2 && testResult3 && testResult4 && testResult5);

    // // Test merging nodes with incorrect conditions
    // graphInfo.mergeNodesMutably(7, {2, 5});
    // bool testResult6 = (graphInfo.inNeigh[2] == std::vector<int>{6});      // Node 2 should remain the same
    // bool testResult7 = (graphInfo.inNeigh[5] == std::vector<int>{4});      // Node 5 should remain the same
    // bool testResult8 = (graphInfo.outNeigh[6] == std::vector<int>{3, 5});  // Node 6 should remain the same
    // bool testResult9 = (graphInfo.outNeigh[7] == std::vector<int>{});      // Node 7 should remain the same
    // runTest("Merge Nodes Test - Incorrect Conditions", testResult6 && testResult7 && testResult8 && testResult9);
}

int main() {
    // Run the test case
    testMergeNodesMutably();

    return 0;
}