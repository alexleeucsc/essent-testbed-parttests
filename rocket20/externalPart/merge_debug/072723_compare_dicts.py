# ------------------------------
# This should take two printout dumps and compare 2 line dumps of listsOfLists
# 
# ------------------------------

import sys
import ast
sys.path.insert(0, '/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/pythonExperiments')
from partEvalFuncs.readGraph_debug import *

def getListOfListOfIDs(line):
    out = []
    split1 = line.split("<1>")[:-1]
    for i in range(len(split1)):
        nxt = []
        neigh = split1[i].split("<2>")[:-1]
        if len(neigh)>0:
            nxt = [int(i) for i in neigh]
        out.append(nxt)
    return out

f1 = open("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/essent/080623_scalalogmssg")
f2 = open("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/essent/080623_cpplogmsg")
flines1 = [l for l in f1]
flines2 = [l for l in f2]
keyword = "mergeSmallPartsDown_mergesToConsider"
IdxList_1 = list(filter(lambda idx : flines1[idx].strip().lower() == keyword.lower(), range(len(flines1))))
IdxList_2 = list(filter(lambda idx : flines2[idx].strip().lower() == keyword.lower(), range(len(flines2))))
check = 0
nestList1 = getListOfListOfIDs(flines1[IdxList_1[check]+1])
nestList2 = getListOfListOfIDs(flines2[IdxList_2[check]+1])
nestList1 = set([tuple(sorted(l)) for l in nestList1])
nestList2 = set([tuple(sorted(l)) for l in nestList2])


#------------------------------get merge memebrs to convert equivilant mergeIDs------------------------------
scala_merge = "/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/071923_mergeSmallParts_merges_scala"
cpp_merge = "/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/080223_mergeSingleInputPartsIntoParents_merges_cpp"
scalaLinesMerge = [l for l in open(scala_merge)]
cppLinesMerge = [l for l in open(cpp_merge)]
MergeIDToMembersIdxList_scala = list(filter(lambda idx : scalaLinesMerge[idx].strip().lower() == "MergeIDToMembers".lower(), range(len(scalaLinesMerge))))
IDToMergeIDIdxList_scala = list(filter(lambda idx : scalaLinesMerge[idx].strip().lower() == "IDToMergeID".lower(), range(len(scalaLinesMerge))))
MergeIDToMembersIdxList_cpp = list(filter(lambda idx : cppLinesMerge[idx].strip().lower() == "MergeIDToMembers".lower(), range(len(cppLinesMerge))))
IDToMergeIDIdxList_cpp = list(filter(lambda idx : cppLinesMerge[idx].strip().lower() == "IDToMergeID".lower(), range(len(cppLinesMerge))))
mergeIDToMembers_scala = getIDToListOfID(scalaLinesMerge[MergeIDToMembersIdxList_scala[check]+1])
IDToMergeID_scala = getListOfID(scalaLinesMerge[IDToMergeIDIdxList_scala[check]+1])
mergeIDToMembers_cpp = getIDToListOfID(cppLinesMerge[MergeIDToMembersIdxList_cpp[check]+1])
IDToMergeID_cpp = getListOfID(cppLinesMerge[IDToMergeIDIdxList_cpp[check]+1])
membersToMergeID_scala = {}
for mergeID, membs in mergeIDToMembers_scala.items():
    membersToMergeID_scala[tuple(sorted(membs))] = mergeID


#------------------------------do tests------------------------------
nestList1_set = set([tuple(sorted(nl)) for nl in nestList1])
wrong = 0
scala_mergeGroups_only = []
cpp_mergeGroups_only = []
for cpp_mergeGroups in nestList2:
    scala_mergeGroups = []
    for m in cpp_mergeGroups:
        scala_mergeGroups.append(membersToMergeID_scala[tuple(sorted(mergeIDToMembers_cpp[m]))])
    if not tuple(sorted(scala_mergeGroups)) in nestList1_set:
        wrong += 1
        if len(cpp_mergeGroups) == 2:
            print("pair")
            print(scala_mergeGroups)
            print(cpp_mergeGroups)
            print("\n")
        scala_mergeGroups_only.append(scala_mergeGroups)
        cpp_mergeGroups_only.append(cpp_mergeGroups)
# print([i in nestList1 for i in nestList2].count(False))
# print([i in nestList2 for i in nestList1].count(False))

# print([i in set([len(l) for l in nestList1]) for i in [len(l) for l in nestList2]].count(False))
# print([i in set([len(l) for l in nestList2]) for i in [len(l) for l in nestList1]].count(False))

