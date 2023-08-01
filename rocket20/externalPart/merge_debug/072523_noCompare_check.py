#
# This script checks that inNeigh and outNeigh are acyclic, and also shows how many partitions have been made
# it also reads in scala info so you can compare the number of mergeGroups
# NOTE: the printout can be from anywhere
#

import sys
import ast
sys.path.insert(0, '/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/pythonExperiments')
from partEvalFuncs.readGraph_debug import *

import sys
import ast
sys.path.insert(0, '/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/pythonExperiments')
from partEvalFuncs.readGraph_debug import *

#------------------------------GET LINE INFO------------------------------
scala_merge = "/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/071923_mergeSmallParts_merges_scala"
scala_neigh = "/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/071923_mergeSmallParts_neighs_scala"
cpp_merge = "/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/071923_mergeSmallParts_merges_cpp"
cpp_neigh = "/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/071923_mergeSmallParts_neighs_cpp"

scalaLinesMerge = [l for l in open(scala_merge)]
scalaLinesNeigh = [l for l in open(scala_neigh)]
InNeighIdxList_scala = list(filter(lambda idx : scalaLinesNeigh[idx].strip().lower() == "InNeigh".lower(), range(len(scalaLinesNeigh))))
OutNeighIdxList_scala = list(filter(lambda idx : scalaLinesNeigh[idx].strip().lower() == "OutNeigh".lower(), range(len(scalaLinesNeigh))))
MergeIDToMembersIdxList_scala = list(filter(lambda idx : scalaLinesMerge[idx].strip().lower() == "MergeIDToMembers".lower(), range(len(scalaLinesMerge))))
IDToMergeIDIdxList_scala = list(filter(lambda idx : scalaLinesMerge[idx].strip().lower() == "IDToMergeID".lower(), range(len(scalaLinesMerge))))

cppLinesMerge = [l for l in open(cpp_merge)]
cppLinesNeigh = [l for l in open(cpp_neigh)]
InNeighIdxList_cpp = list(filter(lambda idx : cppLinesNeigh[idx].strip().lower() == "InNeigh".lower(), range(len(cppLinesNeigh))))
OutNeighIdxList_cpp = list(filter(lambda idx : cppLinesNeigh[idx].strip().lower() == "OutNeigh".lower(), range(len(cppLinesNeigh))))
MergeIDToMembersIdxList_cpp = list(filter(lambda idx : cppLinesMerge[idx].strip().lower() == "MergeIDToMembers".lower(), range(len(cppLinesMerge))))
IDToMergeIDIdxList_cpp = list(filter(lambda idx : cppLinesMerge[idx].strip().lower() == "IDToMergeID".lower(), range(len(cppLinesMerge))))

#------------------------------GET DATASTRUCTS------------------------------
check_scala = 0
mergeIDToMembers_scala = getIDToListOfID(scalaLinesMerge[MergeIDToMembersIdxList_scala[check_scala]+1])
IDToMergeID_scala = getListOfID(scalaLinesMerge[IDToMergeIDIdxList_scala[check_scala]+1])
InNeigh_scala = getIDToListOfIDNeigh(scalaLinesNeigh[InNeighIdxList_scala[check_scala]+1])
OutNeigh_scala = getIDToListOfIDNeigh(scalaLinesNeigh[OutNeighIdxList_scala[check_scala]+1])

check_cpp = 0
mergeIDToMembers_cpp = getIDToListOfID(cppLinesMerge[MergeIDToMembersIdxList_cpp[check_cpp]+1])
IDToMergeID_cpp = getListOfID(cppLinesMerge[IDToMergeIDIdxList_cpp[check_cpp]+1])
InNeigh_cpp = getIDToListOfIDNeigh(cppLinesNeigh[InNeighIdxList_cpp[check_cpp]+1])
OutNeigh_cpp = getIDToListOfIDNeigh(cppLinesNeigh[OutNeighIdxList_cpp[check_cpp]+1])

#------------------------------RUN CYCLE CHECK------------------------------
def is_cyclic(graph):
    def dfs(node, visited, stack):
        visited[node] = True
        stack[node] = True

        for neighbor in graph.get(node, []):
            if not visited[neighbor]:
                if dfs(neighbor, visited, stack):
                    return True
            elif stack[neighbor]:
                return True

        stack[node] = False
        return False

    num_nodes = len(graph)
    visited = {node: False for node in graph}
    stack = {node: False for node in graph}

    for node in graph:
        if not visited[node]:
            if dfs(node, visited, stack):
                return True

    return False

print(is_cyclic(OutNeigh_cpp))