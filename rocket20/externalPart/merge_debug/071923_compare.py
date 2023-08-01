# f1 = open("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/essent/071723_logmssg")
# f2 = open("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/essent/071723_cpplogmsg")
# mffc1 = [l for l in f1]
# mffc2 = [l for l in f2]
# #1: the mffcs are not the same after the first iteration
# newMFFCseeds1 = set([int(i) for i in mffc1[92][:-2].split(" ")])
# newMFFCseeds2 = set([int(i) for i in mffc2[11][:-2].split(" ")])
# # newMFFCseeds1 = set([int(i) for i in mffc1[101][:-2].split(" ")])
# # newMFFCseeds2 = set([int(i) for i in mffc2[17][:-2].split(" ")])
# print(len(newMFFCseeds1), len(newMFFCseeds2))
# print(set(newMFFCseeds1)-set(newMFFCseeds2))
# print(set(newMFFCseeds2)-set(newMFFCseeds1))
# #2: the univistedSinnks are correct, the unvistedFringe is not
# #@2B: it looks like the unvifisted fringe is correct now, but the unvisited fringe isn't?
# unvisitedSinks1 = set([int(i) for i in mffc1[2][:-2].split(" ")])
# all([(i in newMFFCseeds2) for i in unvisitedSinks1])
# unvisitedFringe1 = set([int(i) for i in mffc1[8][:-2].split(" ")])
# all([(i in newMFFCseeds2) for i in unvisitedFringe1])
# #are the excluded the same? it seems so (first visited is only excluded nodes)  idToMergeID
import sys
import ast
sys.path.insert(0, '/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/pythonExperiments')
from partEvalFuncs.readGraph_debug import *
graphObj_scala = graphInfo()
scalaLines = [l for l in open("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/essent/071923_mergeSingleInputPartsIntoParents_scala")]
idxOfMergeIDToMembers_scala = list(filter(lambda idx : scalaLines[idx].strip().lower() == "mergeidtomembers", range(len(scalaLines))))
cppLines = [l for l in open("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/essent/071923_mergeSingleInputPartsIntoParents_scala")]
idxOfMergeIDToMembers_cpp = list(filter(lambda idx : cppLines[idx].strip().lower() == "mergeidtomembers", range(len(cppLines))))

check = 4
MergeIDToMembers_scala = getIDToListOfID(scalaLines[idxOfMergeIDToMembers_scala[check]+1])
MergeIDToMembers_cpp = getIDToListOfID(scalaLines[idxOfMergeIDToMembers_cpp[check]+1])

print(set(MergeIDToMembers_scala.keys()) - set(MergeIDToMembers_cpp.keys()))
print(set(MergeIDToMembers_cpp.keys()) - set(MergeIDToMembers_scala.keys()))
badMergeIDs = list(filter(lambda mergeID : MergeIDToMembers_cpp[mergeID] != MergeIDToMembers_scala[mergeID], MergeIDToMembers_scala.keys()))





idxOfIDToMergeIDs_scala = list(filter(lambda idx : scalaLines[idx].strip().lower() == "idToMergeID".lower(), range(len(scalaLines))))
idxOfIDToMergeIDs_cpp = list(filter(lambda idx : cppLines[idx].strip().lower() == "idToMergeID".lower(), range(len(cppLines))))

check = 4
IDToMergeIDs_scala = getListOfID(scalaLines[idxOfIDToMergeIDs_scala[check]+1])
IDToMergeIDs_cpp = getListOfID(scalaLines[idxOfIDToMergeIDs_cpp[check]+1])

print(set(IDToMergeIDs_scala) - set(IDToMergeIDs_cpp))
print(set(IDToMergeIDs_cpp) - set(IDToMergeIDs_scala))
badMergeIDs = list(filter(lambda mergeID : IDToMergeIDs_cpp[mergeID] != IDToMergeIDs_scala[mergeID], IDToMergeIDs_scala))
print(badMergeIDs)



#------------------------------GET NEIGH INFO------------------------------
scalaLines = [l for l in open("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/essent/071923_mergeSingleInputPartsIntoParents_neighs_scala")]
idxOfInNeigh_scala = list(filter(lambda idx : scalaLines[idx].strip().lower() == "inNeigh".lower(), range(len(scalaLines))))
cppLines = [l for l in open("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/071923_mergeSingleInputPartsIntoParents_neighs_cpp")]
idxOfInNeigh_cpp = list(filter(lambda idx : cppLines[idx].strip().lower() == "inNeigh".lower(), range(len(cppLines))))


#------------------------------COMPARE INNEIGH------------------------------
check = 4
InNeigh_scala = getIDToListOfIDNeigh(scalaLines[idxOfInNeigh_scala[check]+1])
InNeigh_cpp = getIDToListOfIDNeigh(scalaLines[idxOfInNeigh_cpp[check]+1])

print(set(InNeigh_scala) - set(InNeigh_cpp))
print(set(InNeigh_cpp) - set(InNeigh_scala))
badNeighIDs = list(filter(lambda mergeID : InNeigh_cpp[mergeID] != InNeigh_scala[mergeID], InNeigh_scala))
print(badMergeIDs)




#------------------------------GET NEIGH INFO------------------------------
scalaLines = [l for l in open("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/essent/071923_mergeSingleInputPartsIntoParents_neighs_scala")]
idxOfOutNeigh_scala = list(filter(lambda idx : scalaLines[idx].strip().lower() == "OutNeigh".lower(), range(len(scalaLines))))
cppLines = [l for l in open("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/071923_mergeSingleInputPartsIntoParents_neighs_cpp")]
idxOfOutNeigh_cpp = list(filter(lambda idx : cppLines[idx].strip().lower() == "OutNeigh".lower(), range(len(cppLines))))


#------------------------------COMPARE INNEIGH------------------------------
check = 4
OutNeigh_scala = getIDToListOfIDNeigh(scalaLines[idxOfOutNeigh_scala[check]+1])
OutNeigh_cpp = getIDToListOfIDNeigh(scalaLines[idxOfOutNeigh_cpp[check]+1])

print(set(OutNeigh_scala) - set(OutNeigh_cpp))
print(set(OutNeigh_cpp) - set(OutNeigh_scala))
badNeighIDs = list(filter(lambda mergeID : OutNeigh_cpp[mergeID] != OutNeigh_scala[mergeID], OutNeigh_scala))
print(badMergeIDs)
# invalidNodes = set(range(len(graphObj.inNeigh))) - set(graphObj.validNodes)
# visited1 = set([int(i) for i in mffc1[4][:-2].split(" ")])
# len(visited1-invalidNodes)
# #are the fringes the same?
# fringe1 = set([int(i) for i in mffc1[6][:-2].split(" ")])
# [(i in newMFFCseeds2) for i in fringe1]



#------------------------------GET LINE INFO------------------------------
# (1) set scala_logfile to location of scala printout
# (2) set keyword to the word 1 line above the 1-line info printout
scala_logfile = "/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/essent/071923_mergeSingleInputPartsIntoParents_neighs_scala"
cpp_logfile = "/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/071923_mergeSingleInputPartsIntoParents_neighs_cpp"
keyword = "OutNeigh"
scalaLines = [l for l in open(scala_logfile)]
idxList_scala = list(filter(lambda idx : scalaLines[idx].strip().lower() == keyword.lower(), range(len(scalaLines))))
cppLines = [l for l in open(cpp_logfile)]
idxList_cpp = list(filter(lambda idx : cppLines[idx].strip().lower() == keyword.lower(), range(len(cppLines))))

#------------------------------COMPARE INFO------------------------------
check = 4
infodict_scala = getIDToListOfIDNeigh(scalaLines[idxList_scala[check]+1])
infodict_cpp = getIDToListOfIDNeigh(scalaLines[idxList_cpp[check]+1])

print(set(infodict_scala) - set(infodict_cpp))
print(set(infodict_cpp) - set(infodict_scala))
badNeighIDs = list(filter(lambda mergeID : infodict_cpp[mergeID] != infodict_scala[mergeID], infodict_scala))
print(badMergeIDs)