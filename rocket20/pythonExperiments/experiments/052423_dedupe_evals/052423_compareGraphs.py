import sys
sys.path.insert(0, '/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/pythonExperiments/')
#this shows getting IDToOrigSig 
from collections import defaultdict

from partEvalFuncs.dumpData import *
from partEvalFuncs.graphEval import *
from partEvalFuncs.graphProc import *
from partEvalFuncs.partAlgos import *
from partEvalFuncs.readGraph import *
from partEvalFuncs.sanityChecking import *
from partEvalFuncs.unionfind import *

#1: graphObjOrig has info on main branch
graphObjOrig = readGraph("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/essent/rocket18_nondeduped_graphDump")
origSigToID, origIDToSig = getIDToSigFromHarness(graphObjOrig,"/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket18/TestHarness.h")
#assrt len(set(sum(graphObjOrig.inNeigh.values(), start=[]))) 

#2: graphObjDedupe has info on deduped
graphObjSrcListMFFC = readGraph("/soe/alexlee/alexlee/essent-dedupe-experiment/essent/rocket18_deduped_graphDump")
sigToIDSrcList , IDToSigSrcList  = getIDToSigFromHarness(graphObjSrcListMFFC, "/soe/alexlee/alexlee/essent-dedupe-experiment/rocket18/TestHarness.h")
# noInSrcList, noOutSrcList = getNoInNoOut(graphObjSrcListMFFC)
# IDToSrcSrcList = find_sources_sig(graphObjSrcListMFFC.outNeigh, noInSrcList)

#1: inNeighs are diff len?
len(graphObjSrcListMFFC.inNeigh)
len(graphObjOrig.inNeigh)

#1: inNeighs are diff len?
len(graphObjSrcListMFFC.MergeIDToMembers)
len(graphObjOrig.MergeIDToMembers)

#1b: also IDToName shows one has names other doesn't!
set(graphObjSrcListMFFC.IDToName) - set(graphObjOrig.IDToName)
sharedNames = set(graphObjSrcListMFFC.IDToName).intersection(set(graphObjOrig.IDToName))
len(sharedNames)