#include "Graph.hpp"
#include "parseGraph.hpp"
#include "mffc.hpp"
#include "acyclicPart.hpp"

#include <ctime>

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

        //std::cout << "Merging up " << baseSingleInputIDs.size() << " single-input parts" << std::endl;
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

int main(int argc, char* argv[]){
    int smallPartCutoff = 13;
    std::string fileName = "/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/essent/070723_rocket20_k20_graphDump";
    //std::string fileName = "/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/essent/081723_boom21_k20_graphDump";
    fileName = argv[2];
    graphInfo graphObj = processFile(fileName);
    std::vector<int> excluded = graphObj.getExcluded();
    //DEBUG FLAGS
    int debugTimingPerSearch = 0;
    int debugTraversalLists = 0;
    int debugMergeReqSize = 0;
    int debugSearchCountProf = 0;
    graphObj.debugTimingPerSearch = debugTimingPerSearch;
    graphObj.debugTraversalLists = debugTraversalLists;
    //set threshhold size
    graphObj.longSearchThresh = std::stoi(argv[1]);

    //std::cout << "Excluded: ";
    for(auto i:excluded){
        //std::cout << i << ",  ";
    }
    //std::cout << std::endl; 
    std::vector<int> mffc = getMFFCs(graphObj, excluded);
    //std::cout << "mffc: ";
    for(auto i:mffc){
        //std::cout << i << ",  ";
    }
    //std::cout << std::endl;   
    //std::cout << "doneprintout";    
    //std::cout << std::endl;   
    graphObj.applyInitialAssignments(mffc);
    //start partitioning
    //std::set<int> excludedSet(excluded.begin(), excluded.end());
    AcyclicPart ap = AcyclicPart(graphObj, excluded);
    //DEBUG FLAGS
    //ap.debugMergeReqSize = debugMergeReqSize;


    clock_t begin = clock();
    ap.mergeSingleInputPartsIntoParents();
    clock_t end = clock();
    double elapsed_secs = double(end - begin);
    std::cout << "CLOCKS_PER_SEC:" << CLOCKS_PER_SEC << "\n";
    std::cout << "mergeSingleInputPartsIntoParents 1:" << elapsed_secs << "\n";
    std::cout << "mergeSingleInputPartsIntoParents clust count:" << ap.mg.mergeIDToMembers.size();
    std::cout << "\n";
    // if(checkIllegalMemberInMapVals(ap.mg.mergeIDToMembers)){
    //     std::cout<<"illegal node found\n";
    // }

    begin = clock();
    ap.mergeSmallSiblings(smallPartCutoff);
    end = clock();
    elapsed_secs = double(end - begin);
    std::cout << "mergeSmallSiblings 1:" << elapsed_secs << "\n";
    std::cout << "mergeSmallSiblings clust count:" << ap.mg.mergeIDToMembers.size();
    std::cout << "\n";
    if(ap.mg.isCyclic()){std::cout<<"problem\n";exit(111);}
    // if(checkIllegalMemberInMapVals(ap.mg.mergeIDToMembers)){
    //     std::cout<<"illegal node found\n";
    // }

    begin = clock();
    ap.mergeSmallPartsLayered();
    end = clock();
    elapsed_secs = double(end - begin);
    std::cout << "mergeSmallPartsLayered 1:" << elapsed_secs << "\n";
    std::cout << "mergeSmallPartsLayered clust count:" << ap.mg.mergeIDToMembers.size();
    std::cout << "\n";
    std::cout << "\n";
    if(ap.mg.isCyclic()){std::cout<<"problem\n";exit(111);}

    // for(int src=0; src<ap.mg.outNeigh.size(); src++){
    //     for(int dest=0; dest<ap.mg.outNeigh.size(); dest++){
    //         clock_t begin = clock();
    //         ap.mg.mergeIsAcyclic({src, dest});
    //         clock_t end = clock();
    //         double elapsed_secs = double(end - begin);
    //         if(elapsed_secs > 50000){
    //             std::cout << "\t" << src << ":" << dest << ", elapsed_secs " << elapsed_secs << "\n";
    //         }
    //     }
    // }
    // exit(111);

    // ap.mg.mergeIsAcyclicPrintout({90454, 90775});
    // exit(111);
// ap.smallSiblingsTimeTest();
// exit(111);

    begin = clock();
    ap.mergeSmallParts(smallPartCutoff, 0.5);
    end = clock();
    elapsed_secs = double(end - begin);
    std::cout << "mergeSmallParts 1:" << elapsed_secs << "\n";
    std::cout << "mergeSmallParts1 clust count:" << ap.mg.mergeIDToMembers.size();
    std::cout << "\n";
    if(ap.mg.isCyclic()){std::cout<<"problem\n";exit(111);}
    // if(checkIllegalMemberInMapVals(ap.mg.mergeIDToMembers)){
    //     std::cout<<"illegal node found\n";
    // }


    begin = clock();
    ap.mergeSmallPartsDown(smallPartCutoff);
    end = clock();
    elapsed_secs = double(end - begin);
    std::cout << "mergeSmallPartsDown 1:" << elapsed_secs << "\n";
    std::cout << "mergeSmallPartsDown clust count:" << ap.mg.mergeIDToMembers.size();
    std::cout << "\n";
    if(ap.mg.isCyclic()){std::cout<<"problem\n";exit(111);}

    begin = clock();
    ap.mergeSmallParts(2*smallPartCutoff, 0.25);
    end = clock();
    elapsed_secs = double(end - begin);
    std::cout << "mergeSmallParts 2:" << elapsed_secs << "\n";
    std::cout << "mergeSmallParts2 clust count:" << ap.mg.mergeIDToMembers.size();
    std::cout << "\n";
    if(ap.mg.isCyclic()){std::cout<<"problem\n";exit(111);}

    outputMGNeighbors(ap.mg);
    outputMGMergeInfo(ap.mg);
    return 0;
}