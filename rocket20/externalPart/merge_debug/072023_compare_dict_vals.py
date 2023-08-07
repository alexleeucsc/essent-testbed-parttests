import sys
import ast
sys.path.insert(0, '/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/pythonExperiments')
from partEvalFuncs.readGraph_debug import *

#------------------------------GET LINE INFO------------------------------
# (1) set scala_logfile to location of scala printout
# (2) set keyword to the word 1 line above the 1-line info printout
scala_logfile = "/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/071923_mergeSmallSiblings_neighs_scala"
cpp_logfile = "/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/071923_mergeSmallSiblings_neighs_cpp"
keyword = "inNeigh"
scalaLines = [l for l in open(scala_logfile)]
idxList_scala = list(filter(lambda idx : scalaLines[idx].strip().lower() == keyword.lower(), range(len(scalaLines))))
cppLines = [l for l in open(cpp_logfile)]
idxList_cpp = list(filter(lambda idx : cppLines[idx].strip().lower() == keyword.lower(), range(len(cppLines))))

#------------------------------COMPARE INFO------------------------------
check = 0
infodict_scala = getIDToListOfIDNeigh(scalaLines[idxList_scala[check]+1])
infodict_cpp = getIDToListOfIDNeigh(cppLines[idxList_cpp[check]+1])

mergeMemberLists_scala = set(tuple(sorted(membs)) for membs in infodict_scala.values())
print([tuple(sorted(membs)) in mergeMemberLists_scala for membs in infodict_cpp.values()].count(False))
mergeMemberLists_cpp = set(tuple(sorted(membs)) for membs in infodict_cpp.values())
mergeInCppOnly = [tuple(sorted(membs)) in mergeMemberLists_cpp for membs in infodict_scala.values()]
print(mergeInCppOnly.count(False))
idxOfCppOnly = list(filter(lambda i : not mergeInCppOnly[i], range(len(mergeInCppOnly))))
print(idxOfCppOnly)