#include "Graph.hpp"
#include "parseGraph.hpp"
#include "mffc.hpp"
#include "acyclicPart.hpp"

class acyclicPart{
    public:
    graphInfo mg;
    std::vector<NodeID> excludeSet;
    std::vector<NodeID> clustersToKeep;
    acyclicPart(graphInfo mg, std::vector<NodeID> excludeSet){
        this->mg = mg;
        this->excludeSet = excludeSet;
    }
    //converted wholsale by chatGPT
    std::vector<NodeID> findSmallParts(int smallPartCutoff){
        std::vector<NodeID> smallParts;
        for (const auto& pair : mg.mergeIDToMembers) {
            NodeID id = pair.first;
            if (mg.nodeSize(id) < smallPartCutoff &&
                //excludeSet.find(id) == excludeSet.end() &&
                std::find(excludeSet.begin(), excludeSet.end(), id) != excludeSet.end() &&
                //clustersToKeep.find(id) == clustersToKeep.end()) {
                std::find(clustersToKeep.begin(), clustersToKeep.end(), id) != clustersToKeep.end()) {
                smallParts.push_back(id);
            }
        }
        return smallParts;
    }
    //converted wholsale by chatGPT
    void mergeSingleInputPartsIntoParents(int smallPartCutoff = 20) {
        std::vector<NodeID> smallPartIDs = findSmallParts(smallPartCutoff);
        std::vector<NodeID> singleInputIDs;
        for (const auto& id : smallPartIDs) {
            if (mg.inNeigh[id].size() == 1) {
                singleInputIDs.push_back(id);
            }
        }
        std::vector<NodeID> singleInputParents;
        for (const auto& id : singleInputIDs) {
            singleInputParents.insert(
                singleInputParents.end(),
                mg.inNeigh[id].begin(),
                mg.inNeigh[id].end()
            );
        }
        std::sort(singleInputParents.begin(), singleInputParents.end());
        singleInputParents.erase(
            std::unique(singleInputParents.begin(), singleInputParents.end()),
            singleInputParents.end()
        );
        std::vector<NodeID> baseSingleInputIDs;
        std::set_difference(
            singleInputIDs.begin(), singleInputIDs.end(),
            singleInputParents.begin(), singleInputParents.end(),
            std::back_inserter(baseSingleInputIDs)
        );

        std::cout << "Merging up " << baseSingleInputIDs.size() << " single-input parts" << std::endl;
        for (const auto& childID : baseSingleInputIDs) {
            NodeID parentID = mg.inNeigh[childID][0];
            if (
                //!excludeSet.count(parentID) && !clustersToKeep.count(parentID)
                !(std::find(excludeSet.begin(), excludeSet.end(), parentID) != excludeSet.end()) && 
                !(std::find(clustersToKeep.begin(), clustersToKeep.end(), parentID) != clustersToKeep.end())
                ) {
                mg.mergeGroups(parentID, std::vector<int>{childID});
            }
        }

        if (baseSingleInputIDs.size() < singleInputIDs.size()) {
            mergeSingleInputPartsIntoParents(smallPartCutoff);
        }
    }
};

int main(){
    int smallPartCutoff = 20;
    std::string fileName = "/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/essent/070723_rocket20_k20_graphDump";
    graphInfo graphObj = processFile(fileName);
    std::vector<int> excluded = graphObj.getExcluded();
    std::cout << "Excluded: ";
    for(auto i:excluded){
        std::cout << i << ",  ";
    }
    std::cout << std::endl; 
    std::vector<int> mffc = getMFFCs(graphObj, excluded);
    std::cout << "mffc: ";
    for(auto i:mffc){
        std::cout << i << ",  ";
    }
    std::cout << std::endl;   
    std::cout << "doneprintout";    
    std::cout << std::endl;   
    graphObj.applyInitialAssignments(mffc);
    //start partitioning
    std::set<int> excludedSet(excluded.begin(), excluded.end());
    AcyclicPart ap = AcyclicPart(graphObj, excludedSet);
    ap.mergeSingleInputPartsIntoParents();
    ap.mergeSmallSiblings(smallPartCutoff);
    ap.mergeSmallParts(smallPartCutoff, 0.5);
    ap.mergeSmallPartsDown(smallPartCutoff);
    ap.mergeSmallParts(smallPartCutoff, 0.25);
    return 0;
}