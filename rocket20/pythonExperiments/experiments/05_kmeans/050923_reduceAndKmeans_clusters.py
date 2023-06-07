#this shows getting IDToOrigSig 

from graphFunctions import *
from collections import defaultdict
from unionfind import UnionFind
from sklearn import decomposition
from sklearn.cluster import KMeans
from sklearn.cluster import MiniBatchKMeans
import numpy as np

import sys
print (sys.argv)
clusterCount = int(sys.argv[1])
print(clusterCount)
import time
#get orig info
graphObjOrig = graphInfo()
graphObjOrig = readGraph(graphObjOrig, "/soe/alexlee/alexlee/essent-testbed/essent/050423_orig")
origSigToID, origIDToSig = getIDToSigFromHarness(graphObjOrig,"/soe/alexlee/alexlee/essent-testbed/rocket20/TestHarness.h")
activationList = getActivationList("/soe/alexlee/alexlee/essent-testbed/rocket20/activationDump.txt", 2000)
#partitionList = getPartActList("/soe/alexlee/alexlee/essent-testbed/rocket20/partitionDump.txt", 2000)
noInSrcList, noOutSrcList = getNoInNoOut(graphObjOrig)
IDToSrcSrcList = find_sources_sig(graphObjOrig.outNeigh, noInSrcList)



#1: do pca
print("done reading graph, start pruning 0")
origNumberOfNodes = len(activationList[0])
allNodeIdx = np.array(range(origNumberOfNodes))
activationListSplit = [[int(i) for i in l] for l in activationList]
activationListSplit = np.array(activationListSplit)

idx_delNodes = np.argwhere(np.all(activationListSplit[..., :] == 0, axis=0))
idx_remaining = np.delete(allNodeIdx, idx_delNodes, axis=0)
assert(len(set(idx_remaining).union(set(idx_delNodes.flatten()))) == origNumberOfNodes)
activationListShort = np.delete(activationListSplit, idx_delNodes, axis=1)

activationListShortTrans = np.transpose(activationListShort)

idx = np.argwhere(np.all(activationListShortTrans[..., :] == 0, axis=0))
activationListShort = np.delete(activationListShortTrans, idx, axis=1)
print("done pruning 0s, start kmeans")

# pca = decomposition.PCA(n_components=1000)
# t0 = time.time()
# activationListShort = pca.fit_transform(activationListShort)
# t1 = time.time()
# print("PCA overhead:",t1-t0)

#debug 052523
#kmeans = KMeans(n_clusters=8800, random_state=0, n_init="auto").fit(activationListShort)
t0 = time.time()
kmeans = MiniBatchKMeans(n_clusters=clusterCount,random_state=0,batch_size=6,max_iter=100,n_init="auto").fit(activationListShort)
t1 = time.time()
print("kmeans overhead:",t1-t0)
#IDToMergeID2 = kmeans.labels_
IDToMergeID2 = np.array(range(origNumberOfNodes))
new_label = max(kmeans.labels_)+1
for idx in range(len(idx_remaining)):
    IDToMergeID2[idx_remaining[idx]] = kmeans.labels_[idx]
for idx in idx_delNodes:
    IDToMergeID2[idx] = new_label
MergeIDToMembers2 = {}
for ID in range(len(IDToMergeID2)):
    if not IDToMergeID2[ID] in MergeIDToMembers2:
        MergeIDToMembers2[IDToMergeID2[ID]] = [ID]
    else:
        MergeIDToMembers2[IDToMergeID2[ID]].append(ID)
#MergeIDToMembers2[max(MergeIDToMembers2.keys())+1] = [i for i in idx_delNodes.flatten()]

#BUG 051123 - we are clustering using nodes represented in activationLists:
#1: rename each sig to node
MergeIDToMembers2_ID = {}
for mergeID, mergeMems in MergeIDToMembers2.items():
    #b: map each mergeMems sigIdx back to a sig
    mergeMems = list(filter(lambda Sig : Sig in origSigToID, mergeMems))
    sigMergeMembs = list(map(lambda Sig : origSigToID[Sig], mergeMems))
    MergeIDToMembers2_ID[mergeID] = sigMergeMembs
MergeIDToMembers2 = MergeIDToMembers2_ID
#2: add back each node not related to a sig
allNodes = len(graphObjOrig.outNeigh)
NodesWithSig = set(sum(MergeIDToMembers2.values(),start=[]))
noSigNodes = list(filter(lambda ID:not ID in NodesWithSig, range(allNodes)))
MergeIDToMembers2[max(MergeIDToMembers2.keys())+1] = noSigNodes

# MergeIDToMembers2 = {}
# for ID in graphObjOrig.inNeigh:
#     MergeIDToMembers2[ID] = [ID]

IDToMergeID2 = {}
for mergeID, mergeMembers in MergeIDToMembers2.items():
    for memb in mergeMembers:
        IDToMergeID2[memb] = mergeID
graphObjOrig.IDToMergeID = IDToMergeID2
graphObjOrig.MergeIDToMembers = MergeIDToMembers2


evaluateClusters(graphObjOrig, graphObjOrig.IDToMergeID, graphObjOrig.MergeIDToMembers, activationList, origSigToID, origIDToSig)

#debug
# myarr = np.array([[1,0,0,1],[1,0,1,0]])
# print(myarr[..., :])
# print(np.all(myarr[..., :], axis=0))
# print(np.delete(myarr, np.array([True, False, False, False]), axis=1))

# myarr = np.array([[1,0,0,1],[0,0,0,0],[1,0,1,0],[0,0,0,0]])
# print(myarr[..., :])
# print(np.all(myarr[..., :], axis=1))
# print(np.delete(myarr, np.array([True, False, False, False]), axis=1))