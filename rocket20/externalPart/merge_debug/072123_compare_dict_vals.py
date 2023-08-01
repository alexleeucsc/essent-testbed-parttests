#
# This script compares inNeigh from cpp and scala taking account for the fact that merge groups can have different names but the same members
# it reads the merge groups AND the neighbors from cpp and scala
# NOTE: the printout can be from anywhere
#

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
check = 0
mergeIDToMembers_scala = getIDToListOfID(scalaLinesMerge[MergeIDToMembersIdxList_scala[check]+1])
IDToMergeID_scala = getListOfID(scalaLinesMerge[IDToMergeIDIdxList_scala[check]+1])
InNeigh_scala = getIDToListOfIDNeigh(scalaLinesNeigh[InNeighIdxList_scala[check]+1])
OutNeigh_scala = getIDToListOfIDNeigh(scalaLinesNeigh[OutNeighIdxList_scala[check]+1])

mergeIDToMembers_cpp = getIDToListOfID(cppLinesMerge[MergeIDToMembersIdxList_cpp[check]+1])
IDToMergeID_cpp = getListOfID(cppLinesMerge[IDToMergeIDIdxList_cpp[check]+1])
InNeigh_cpp = getIDToListOfIDNeigh(cppLinesNeigh[InNeighIdxList_cpp[check]+1])
OutNeigh_cpp = getIDToListOfIDNeigh(cppLinesNeigh[OutNeighIdxList_cpp[check]+1])

#------------------------------COMPARE STRUCTS------------------------------
#santiy check: do mergeIDToMembers_cpp.values and mergeIDToMembers_scala.values have mostly the same vals?
mid2m_valSet_cpp = [tuple(sorted(s)) for s in mergeIDToMembers_cpp.values()]
mid2m_valSet_scala = [tuple(sorted(s)) for s in mergeIDToMembers_scala.values()]
#print([s in mid2m_valSet_cpp for s in mid2m_valSet_scala].count(False))
#1
membersToMergeID_scala = {}
for mergeID, membs in mergeIDToMembers_scala.items():
    membersToMergeID_scala[tuple(sorted(membs))] = mergeID

#2 check inNeigh
neigh_to_check_cpp_l = []
mergeID_cpp_l = []
for mergeID_cpp, membs_cpp in mergeIDToMembers_cpp.items():
    #get equivilant scala node
    if (tuple(sorted(membs_cpp)) in membersToMergeID_scala):
        mergeID_scala = membersToMergeID_scala[tuple(sorted(membs_cpp))]
        #iter over cpp outNeighs
        for neigh_to_check_cpp in InNeigh_cpp[mergeID_cpp]:
            if tuple(sorted(mergeIDToMembers_cpp[neigh_to_check_cpp])) in membersToMergeID_scala:
                neigh_to_checkAgainst_scala = membersToMergeID_scala[tuple(sorted(mergeIDToMembers_cpp[neigh_to_check_cpp]))]
                if not (neigh_to_checkAgainst_scala in InNeigh_scala[mergeID_scala]):
                    print("")
                    print(mergeID_cpp, membs_cpp)
                    print(mergeID_scala)
                    exit(111)
            else:
                print("membs not shared 1: ", neigh_to_check_cpp)
                neigh_to_check_cpp_l.append(neigh_to_check_cpp)
    else:
        print("membs not shared 2",mergeID_cpp)
        mergeID_cpp_l.append(mergeID_cpp)

#3 looks like mergeIDToMembers_cpp[64211] has members that aren't in membersToMergeID_scala on check = 1
    #it also seems like mergeIDToMembers_scala has these memebrs in their own mergeIDs
#3.1 (assert check = 0 has exact same graphs) true
#3.2 break in cpp version for when attempting to merge 64211
