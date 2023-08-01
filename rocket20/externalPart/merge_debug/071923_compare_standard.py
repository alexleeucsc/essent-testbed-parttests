import sys
import ast
sys.path.insert(0, '/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/pythonExperiments')
#from partEvalFuncs.readGraph_debug import *

def getIDToListOfIDNeigh(line):
    neighDict = {}
    split1 = line.split("<1>")[:-1]
    for i in range(len(split1)):
        graphID_plus_neigh = split1[i].split("<2>")[:-1]
        if len(graphID_plus_neigh)>1:
            graphID, neigh = graphID_plus_neigh[0], graphID_plus_neigh[1:]
            neigh = [int(i) for i in neigh]
            neighDict[int(graphID)] = neigh
        else:
            graphID = graphID_plus_neigh[0]
            neighDict[int(graphID)] = []
    return neighDict

#------------------------------GET LINE INFO------------------------------
# (1) set scala_logfile to location of scala printout
# (2) set keyword to the word 1 line above the 1-line info printout
scala_logfile = "/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/071923_mergeSmallSiblings_merges_scala"
cpp_logfile = "/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/071923_mergeSmallSiblings_merges_cpp"
keyword = "mergeIDToMembers"
scalaLines = [l for l in open(scala_logfile)]
idxList_scala = list(filter(lambda idx : scalaLines[idx].strip().lower() == keyword.lower(), range(len(scalaLines))))
cppLines = [l for l in open(cpp_logfile)]
idxList_cpp = list(filter(lambda idx : cppLines[idx].strip().lower() == keyword.lower(), range(len(cppLines))))

#------------------------------COMPARE INFO------------------------------
check = 4
infodict_scala = getIDToListOfIDNeigh(scalaLines[idxList_scala[check]+1])
infodict_cpp = getIDToListOfIDNeigh(cppLines[idxList_cpp[check]+1])

print(set(infodict_scala) - set(infodict_cpp))
print(set(infodict_cpp) - set(infodict_scala))
#badIDs = list(filter(lambda mergeID : infodict_cpp[mergeID] != infodict_scala[mergeID], infodict_scala))
#1: this isn't a useful metric: the same merge group might have different ID names
#badIDs = list(filter(lambda mergeID : infodict_cpp[mergeID] != infodict_scala[mergeID], infodict_cpp))
#print(badIDs)
infodict_scala_vals = set([tuple(sorted(i)) for i in infodict_scala.values()])
infodict_cpp_vals = set([tuple(sorted(i)) for i in infodict_cpp.values()])
print(infodict_scala_vals - infodict_cpp_vals)
print(infodict_cpp_vals - infodict_scala_vals)