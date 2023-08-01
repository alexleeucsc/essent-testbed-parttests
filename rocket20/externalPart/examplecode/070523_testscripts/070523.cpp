#include <vector>
#include <algorithm>
#include <unordered_set>

using NodeID = int;
using Graph = std::vector<std::vector<NodeID>>;

std::vector<NodeID> combineInNeigh(const std::vector<NodeID>& idsToMerge, const Graph& inNeigh) {
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

#include <iostream>
#include <cassert>

// Test harness
void runTest(const std::string& testName, bool result) {
    std::cout << testName << ": " << (result ? "Passed" : "Failed") << std::endl;
}

// Test case for combineInNeigh function
void testCombineInNeigh() {
    // Sample input
    std::vector<NodeID> idsToMerge = {1, 2};
    Graph inNeigh = {{}, {2, 3, 4}, {1, 3, 5}, {2, 4, 5}, {1, 3, 5}};
    
    // Expected output
    std::vector<NodeID> expected = {3, 4, 5};
    
    // Call the function
    std::vector<NodeID> result = combineInNeigh(idsToMerge, inNeigh);
    
    // Compare the result with the expected output
    bool testResult = (result == expected);
    
    // Run the test
    runTest("combineInNeigh Test", testResult);
}

int main() {
    // Run the test case
    testCombineInNeigh();
    
    return 0;
}