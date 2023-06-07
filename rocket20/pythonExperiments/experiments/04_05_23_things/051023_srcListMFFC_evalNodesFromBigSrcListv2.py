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

#1: graphObjOrig has info about MFFC only graph
#also need harness from original test harness
graphObjOrig = readGraph("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/essent/051723_original_graphdump")
origSigToID, origIDToSig = getIDToSigFromHarness(graphObjOrig,"/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/TestHarness_orig.h")
#assrt len(set(sum(graphObjOrig.inNeigh.values(), start=[]))) 

#2: graphObjSrcListMFFC has info about srcList + MFFC
#also need harness from new clustering
graphObjSrcListMFFC = readGraph("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/essent/051723_srcListPlusOrig_graphdump")
sigToIDSrcList , IDToSigSrcList  = getIDToSigFromHarness(graphObjSrcListMFFC, "/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/TestHarness.h")
noInSrcList, noOutSrcList = getNoInNoOut(graphObjSrcListMFFC)
IDToSrcSrcList = find_sources_sig(graphObjSrcListMFFC.outNeigh, noInSrcList)

#3: graphObjSrcOnly_big starts with original info
#the only info we retain is neighbors, names, and sigs
#then, we set the mergeID clusters using the getSrcClusters func
#also need harness from new clustering
graphObjSrcOnly_big = readGraph("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/essent/051723_original_graphdump")
sigToIDSrcOnly_big, IDToSigSrcOnly_big = getIDToSigFromHarness(graphObjSrcOnly_big,"/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/TestHarness.h")
noInSrcOnly_big, noOutSrcOnly_big = getNoInNoOut(graphObjSrcOnly_big)
IDToSrcSrcOnly_big = find_sources_sig(graphObjSrcOnly_big.outNeigh, noInSrcOnly_big)
#3b: get new clusters and set them
IDToMergeID2SrcOnly_big, MergeIDToMembers2SrcOnly_big = getSharedSrcsCluster(IDToSrcSrcOnly_big, graphObjSrcOnly_big)
graphObjSrcOnly_big.IDToMergeID = IDToMergeID2SrcOnly_big
graphObjSrcOnly_big.MergeIDToMembers = MergeIDToMembers2SrcOnly_big

#3: activaiton list is from orig, but shoudl be same everywhere
activationList = getActivationList("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/activationDump.txt", 2000)

#4: get big clusters from graphObjSrcListMFFC
relevantClusters_srcListMFFC = list(filter(lambda pair:len(pair[1])>8, graphObjSrcListMFFC.MergeIDToMembers.items()))
MergeIDToMembers_bigOnly_srcListMFFC = {ID:graphObjSrcListMFFC.MergeIDToMembers[ID] for ID,members in relevantClusters_srcListMFFC}
relevantClusterIDs_srcListMFFC = [i[0] for i in relevantClusters_srcListMFFC]
relevantClusterMembers_srcListMFFC = [sorted(i[1]) for i in relevantClusters_srcListMFFC]
MergeIDToMembers_srcListMFFC_big = {ID:graphObjSrcListMFFC.MergeIDToMembers[ID] for ID,members in relevantClusters_srcListMFFC}



relevantClusters_SrcOnly_big = list(filter(lambda pair:len(pair[1])>8, graphObjSrcOnly_big.MergeIDToMembers.items()))
relevantClusterMembers_SrcOnly_big = [sorted(i[1]) for i in relevantClusters_SrcOnly_big]
MergeIDToMembers_SrcOnly_big = {ID:graphObjSrcOnly_big.MergeIDToMembers[ID] for ID,members in relevantClusters_SrcOnly_big}

relevantClusters_SrcOnly_small = list(filter(lambda pair:len(pair[1])<=8, graphObjSrcOnly_big.MergeIDToMembers.items()))
relevantClusterMembers_SrcOnly_small = [sorted(i[1]) for i in relevantClusters_SrcOnly_small]
MergeIDToMembers_SrcOnly_small = {ID:graphObjSrcOnly_big.MergeIDToMembers[ID] for ID,members in relevantClusters_SrcOnly_small}

#1b: assert that big clusters after srcList+MFFC are the same as the big clusters only after srcList
#(check against srcList clusters from this file and from ext)
#(1) relevantClusters_SrcOnly_big consists of merge clusters from func above, w/ node IDs from orig
#(2) MergeIDToMembers_bigOnly_afterMFFC consists of merge clusters from external, which are created w/ srcList+MFFC node IDs
check1 = all([sorted(i[1]) in relevantClusterMembers_srcListMFFC for i in relevantClusters_SrcOnly_big])
print(check1)
#assert that the big clusters after srcList+MFFC have the same margeIDs as well
check2 = False
for mergeID, mergeMembers in relevantClusters_SrcOnly_big:
    idx = relevantClusterMembers_srcListMFFC.index(sorted(mergeMembers))
    print(mergeID == relevantClusterIDs_srcListMFFC[idx])
    assert(mergeID == relevantClusterIDs_srcListMFFC[idx])

#2: assert that big clusters only from srcList file performs as well as big clusters only from srcList algo directly
#eval with only ID data from srcList+MFFC clustering
#MergeIDToMembers_SrcOnly_big has same members as relevantClusterMembers_srcListMFFC

#
def evalFuncSuite(): 
    print("@Eval srcList (too many node version, should have good activation ratio)")
    evaluatePartialClusters(graphObjSrcOnly_big, graphObjSrcOnly_big.IDToMergeID, graphObjSrcOnly_big.MergeIDToMembers, activationList, sigToIDSrcList, IDToSigSrcList)

    print("@Eval only clusters of size > 8 from srcList")
    evaluatePartialClusters(graphObjSrcOnly_big, graphObjSrcOnly_big.IDToMergeID, MergeIDToMembers_SrcOnly_big, activationList, sigToIDSrcList, IDToSigSrcList)

    print("@Eval only clusters of size > 8 from srcList+MFFC (should be worse, since it containts clusters > 8 that were introduced MFFC)")
    evaluatePartialClusters(graphObjSrcListMFFC, graphObjSrcListMFFC.IDToMergeID, MergeIDToMembers_srcListMFFC_big, activationList, sigToIDSrcList, IDToSigSrcList)

    print("@Eval only clusters of size > 8 EXCLUDING new clusters formed by MFFC, in the srcList+MFFC cluster graph (should be the same as srcList only)")
    evaluatePartialClusters(graphObjSrcListMFFC, graphObjSrcListMFFC.IDToMergeID, MergeIDToMembers_SrcOnly_big, activationList, sigToIDSrcList, IDToSigSrcList)

    print("@Eval only clusters of size > 8 from srcList+MFFC + double check that graphObj doens't matter (should be the same as from srcListMFFC)")
    evaluatePartialClusters(graphObjSrcListMFFC, graphObjSrcListMFFC.IDToMergeID, MergeIDToMembers_srcListMFFC_big, activationList, sigToIDSrcList, IDToSigSrcList)
evalFuncSuite()

# specNodes = sum(relevantClusterMembers_SrcOnly_big, start=[])
# print("@Eval big nodes")
# evaluateSpecNodes(graphObjOrig, graphObjOrig.IDToMergeID, graphObjOrig.MergeIDToMembers, specNodes, activationList, origSigToID, origIDToSig)
# print("@Eval small nodes")
# specNodes = sum(relevantClusterMembers_SrcOnly_small, start=[])
# evaluateSpecNodes(graphObjOrig, graphObjOrig.IDToMergeID, graphObjOrig.MergeIDToMembers, specNodes, activationList, origSigToID, origIDToSig)
# print("@Eval total:")
# evaluateClusters(graphObjOrig, graphObjOrig.IDToMergeID, graphObjOrig.MergeIDToMembers, activationList, origSigToID, origIDToSig)
#3: assert that nodes in big clusters only from srcList perform as well in srcList clustering as they do in the original clustering


#debug:
