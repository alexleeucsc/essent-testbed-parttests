#include <tuple>
#include <vector>
#include <set>
#include <algorithm>
#include <deque>
#include <map>
#include <iostream>

std::tuple<bool, std::vector<int>> mergeIsAcyclicTopo(std::set<int> ids,
                                                        std::vector<int>& nodeToTopoIndex,
                                                        std::vector<int>& topoSort,
                                                        std::vector<std::vector<int>>& outNeigh,
                                                        std::map<int, std::vector<int>>& mergeIDToMembers){
    int shortSearchedNodes = 1;
    
    std::vector<int> ids_topoIdx;
    ids_topoIdx.reserve(ids.size());
    for(auto n : ids){
        ids_topoIdx.push_back(nodeToTopoIndex[n]);
    }
    //std::cout<<"ids_topoIds:"<<std::endl;
    for(auto n : ids_topoIdx){
        //std::cout<<n<<", ";
    }
    //std::cout<<"\nids_topoIds done"<<std::endl;
    std::set<int> visitedSublist;
    //sort ids by their corresponding nodeToTopoIndex value
    std::vector<int> ids_sortedByTopo_inverted(ids.begin(), ids.end());
    std::sort(ids_sortedByTopo_inverted.begin(), ids_sortedByTopo_inverted.end(),
    [&](int id1, int id2) {
        return nodeToTopoIndex[id1] > nodeToTopoIndex[id2];
    });
    std::vector<int> idsToMoveBack;
    //std::cout<<"mergeIsAcyclicTopo\n";
    for (auto idToProc : ids_sortedByTopo_inverted){
        std::deque<int> init_frontier = {idToProc};
        std::deque<int> frontier;
        for (const auto& source : init_frontier) {
            const auto& neighbors = outNeigh[source];
            for (const auto& neigh : neighbors) {
                if (neigh != idToProc &
                    std::find(ids_sortedByTopo_inverted.begin(), ids_sortedByTopo_inverted.end(), neigh) == ids_sortedByTopo_inverted.end() & 
                    nodeToTopoIndex[neigh]<*std::max_element(ids_topoIdx.begin(), ids_topoIdx.end())) {
                    //std::cout << nodeToTopoIndex[neigh] << ", ";
                    if(nodeToTopoIndex[neigh]<*std::min_element(ids_topoIdx.begin(), ids_topoIdx.end())){
                        std::cout << "this shouldn't happen\n";
                    }
                    visitedSublist.insert(neigh);
                    frontier.push_back(neigh);
                    //DEBUG 091923
                    if(std::find(idsToMoveBack.begin(),idsToMoveBack.end(),neigh) != idsToMoveBack.end()){
                        std::cout<<"re adding\n";
                    }
                    idsToMoveBack.push_back(neigh);
                }
            }
        }
        //std::deque<int> frontier = {idToProc};
        visitedSublist.insert(idToProc);
        while(frontier.size() > 0){
            int nextVal = frontier.back();
            frontier.pop_back();
            for (auto c : outNeigh[nextVal]){
                shortSearchedNodes += 1;
                if(ids.find(c) != ids.end()){
                    //std::cout << "shortSearchedNodes:" << shortSearchedNodes << "\t" << "false" << std::endl;
                    return std::tuple<bool, std::vector<int>>(false, std::vector<int>{});
                }
                if(visitedSublist.find(c) == visitedSublist.end()){
                    visitedSublist.insert(c);
                    if(nodeToTopoIndex[c]<*std::max_element(ids_topoIdx.begin(), ids_topoIdx.end())){
                        //std::cout << nodeToTopoIndex[c] << ", ";
                        if(nodeToTopoIndex[c]<*std::min_element(ids_topoIdx.begin(), ids_topoIdx.end())){
                            std::cout << "this shouldn't happen\n";
                        }
                        frontier.push_front(c);
                        //DEBUG 091923
                        if(std::find(idsToMoveBack.begin(),idsToMoveBack.end(),c) != idsToMoveBack.end()){
                            std::cout<<"re adding\n";
                        }
                        idsToMoveBack.push_back(c);
                    }
                }
            }
        }
    }
    //std::cout<<"\n";
    
    int topoRangeMin = *std::min_element(ids_topoIdx.begin(), ids_topoIdx.end());
    int topoRangeMax = *std::max_element(ids_topoIdx.begin(), ids_topoIdx.end());
    std::vector<int> remainingNodes;
    remainingNodes.reserve(ids.size());
    for(int i = topoRangeMin; i < topoRangeMax+1; i++){
        if( std::find(idsToMoveBack.begin(), idsToMoveBack.end(), topoSort[i]) == idsToMoveBack.end() &&
            std::find(ids.begin(), ids.end(), topoSort[i]) == ids.end() && 
            mergeIDToMembers.find(topoSort[i]) != mergeIDToMembers.end()){

            //DEBUG 091923    
            if(std::find(remainingNodes.begin(), remainingNodes.end(), topoSort[i]) != remainingNodes.end()){
                std::cout<<"re-adding to remaining\n";
            }
            // if(topoSort[i] == 15308){
            //     std::cout<<"special case break";
            // }
            remainingNodes.push_back(topoSort[i]);
        }
    }
    std::sort(idsToMoveBack.begin(), idsToMoveBack.end(),
    [&](int id1, int id2) {
        return nodeToTopoIndex[id1] < nodeToTopoIndex[id2];
    });
    std::sort(remainingNodes.begin(), remainingNodes.end(),
    [&](int id1, int id2) {
        return nodeToTopoIndex[id1] < nodeToTopoIndex[id2];
    });
    std::vector<int> idListNew;
    // idListNew.insert(idListNew.end(), idsToMoveBack.begin(), idsToMoveBack.end());
    // idListNew.push_back(*std::max_element(ids.begin(), ids.end()));
    // idListNew.insert(idListNew.end(), remainingNodes.begin(), remainingNodes.end());
    idListNew.insert(idListNew.end(), remainingNodes.begin(), remainingNodes.end());
    idListNew.push_back(*std::max_element(ids.begin(), ids.end()));
    idListNew.insert(idListNew.end(), idsToMoveBack.begin(), idsToMoveBack.end());

    //DEBUG 091923
    std::set<int> idListNewCopy;
    for(auto i : idListNew){
        if(idListNewCopy.find(i) == idListNewCopy.end()){
            idListNewCopy.insert(i);
        }else{
            std::cout<<"re added\n";
        }
    }
    //std::cout << "shortSearchedNodes:" << shortSearchedNodes << "\t" << "true" << std::endl;
    return std::tuple<bool, std::vector<int>> (true, idListNew);
}