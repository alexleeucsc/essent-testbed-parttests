import sys
sys.path.insert(0, '/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/pythonExperiments/')

from collections import defaultdict

from partEvalFuncs.dumpData import *
from partEvalFuncs.graphEval import *
from partEvalFuncs.graphProc import *
from partEvalFuncs.partAlgos import *
from partEvalFuncs.readGraph import *
from partEvalFuncs.sanityChecking import *
from partEvalFuncs.unionfind import *

graphObj = readGraph("/soe/alexlee/alexlee/essent-testbed/essent/050423_orig")
#activationList = getActivationList("/soe/alexlee/alexlee/essent-testbed/rocket20/activationDump.txt", 2000)
#partitionList = getPartActList("/soe/alexlee/alexlee/essent-testbed/rocket20/partitionDump.txt", 2000)
sigToID, IDToSig = getIDToSigFromHarness(graphObj,"/soe/alexlee/alexlee/essent-testbed/rocket20/TestHarness.h")
noIn, noOut = getNoInNoOut(graphObj)
IDToSrc = find_sources_sig(graphObj.outNeigh, noIn)

#BUG 42023: bandaid on bigger error here:
# for ID,outNeighs in graphInfo.outNeigh.items():
#     for outNeigh in outNeighs:
#         if not ID in graphInfo.inNeigh[outNeigh]:
#             graphInfo.inNeigh[outNeigh].append(ID)

graphObj.validNodes = [int(i) for i in graphObj.validNodes[:-1]]

def getSharedSrcsCluster(IDToSrc, graphObj):
    #1: assign IDs to shared_src, then BFS to split non-shared
    #1b: only put in valid nodes, and name each cluster after a member
    MergeIDToMembers2 = defaultdict(list)
    srcTupleToMegeID = {}
    mergeID = 0
    for ID, srcList in IDToSrc.items():
        if ID in graphObj.validNodes:
            srcTuple = tuple(srcList)
            if srcTuple in srcTupleToMegeID:
                MergeIDToMembers2[srcTupleToMegeID[srcTuple]].append(ID)
            else:
                srcTupleToMegeID[srcTuple] = ID
                MergeIDToMembers2[ID].append(ID)
    #1b: split unconnected mergeIDs
    MergeIDToMembers2_toRem = defaultdict(list)
    MergeIDToMembers2_toAdd = defaultdict(list)
    for toProcMergeID, members in MergeIDToMembers2.items():
        uf = UnionFind(members)
        for memb in members:
            for inNeigh in graphObj.inNeigh[memb]:
                if inNeigh in members:
                    uf.union(memb,inNeigh)
            for outNeigh in graphObj.outNeigh[memb]:
                if outNeigh in members:
                    uf.union(memb,outNeigh)
        
        for i in range(0,len(uf.components())):
            toRemove = uf.components()[i]
            if not toProcMergeID in toRemove:
                for IDToRemove in toRemove:
                    MergeIDToMembers2_toRem[toProcMergeID].append(IDToRemove)
                #possible 0426 bug? (not eq, use append)
                for IDToRemove in toRemove:
                    MergeIDToMembers2_toAdd[list(toRemove)[0]].append(IDToRemove)
    for toRemMergeID, toRemMembers in MergeIDToMembers2_toRem.items():
        for toRemMemb in toRemMembers:
            MergeIDToMembers2[toRemMergeID].remove(toRemMemb)
    for toAddMergeID, toAddMembers in MergeIDToMembers2_toAdd.items():
        for toAddMemb in toAddMembers:
            MergeIDToMembers2[toAddMergeID].append(toAddMemb)
    # #2: to adhere to essent assertions, name each mergeGroup a memberID
    # validSet = set(graphInfo.validNodes)
    # print(validSet)
    # MergeIDToMembers2Copy = {}
    # for mergeID, members in MergeIDToMembers2.items():
    #     validMembers = list(filter(lambda x:x in validSet, members))
    #     print(validMembers)
    #     if len(validMembers)>0:
    #         #print("valid!")
    #         MergeIDToMembers2Copy[validMembers[0]] = members
    #     #print("invalid!")
    # MergeIDToMembers2 = MergeIDToMembers2Copy
    IDToMergeID2 = {}
    for mergeID, members in MergeIDToMembers2.items():
        for memb in members:
            IDToMergeID2[memb] = mergeID
    #3: put each invalid node into it's own partition
    validSet = set(graphObj.validNodes)
    for node in IDToSrc:
        if not node in validSet:
            assert(node not in MergeIDToMembers2)
            MergeIDToMembers2[node].append(node)
            IDToMergeID2[node] = node
    return IDToMergeID2, MergeIDToMembers2

IDToMergeID2, MergeIDToMembers2 = getSharedSrcsCluster(IDToSrc, graphObj)

#042623 sanity check:
values = sum(MergeIDToMembers2.values(),start=[])
for i in range(max(values)):
    if not i in values:
        assert(False)
        print(i)

for mergeID, members in MergeIDToMembers2.items():
    if not (mergeID in members):
        print(mergeID, members)


for mergeID, members in MergeIDToMembers2.items():
    for mem in members:
        if not (IDToMergeID2[mem]==mergeID):
            print(mergeID, members)


#BUG? 0426: should be mergeID first...
onlyGetClusters = True
if onlyGetClusters:
    mergeIDDumpFile = open("042523_bigclusters","w")
    for ID, mergeID in IDToMergeID2.items():
        mergeIDDumpFile.write(str(ID))
        mergeIDDumpFile.write(" ")
        mergeIDDumpFile.write(str(mergeID))
        mergeIDDumpFile.write("\n")
    mergeIDDumpFile.close()
    exit(111)

IDToMergeID_orig = graphInfo.IDToMergeID
MergeIDToMembers_orig = graphInfo.MergeIDToMembers
graphInfo.IDToMergeID = IDToMergeID2
graphInfo.MergeIDToMembers = MergeIDToMembers2
#also remember to recalc boundary vals

clusterToOutputIDs, clusterToInputIDs = getBoundaries(graphInfo.outNeigh, IDToMergeID2, noIn, noOut)
#042023:
# 1: turn clustered graph into graph; turn larges clustes we want to keep into "big nodes"
#new graph data structs
cluster1_inNeighs = {}
cluster1_outNeighs = {}
#link from big node to members
# BUG: see 42023
# bigNodeIDToMembers = {}
# IDtoBigNode = {}
# for mergeID, members in MergeIDToMembers2.items():
#     if len(members) > 8:
#         totalOutput = []
#         rand_mem = members.pop()
#         for outputID in clusterToOutputIDs[mergeID]:
#             totalOutput += graphInfo.outNeigh[outputID]
#         cluster1_outNeighs[rand_mem] = totalOutput
#         totalInput = []
#         for inputID in clusterToInputIDs[mergeID]:
#             totalInput += graphInfo.inNeigh[inputID]
#         cluster1_inNeighs[rand_mem] = totalInput
#         bigNodeIDToMembers[rand_mem] = members
#         for mem in members:
#             IDtoBigNode[mem] = rand_mem
#     else:
#         for member in members:
#             cluster1_outNeighs[member] = []
#             cluster1_inNeighs[member] = []
#             # if outNeighs are in a cluster, set outNeigh to that cluster
#             for outNeigh in graphInfo.outNeigh[member]:
#                 if outNeigh in IDtoBigNode:
#                     cluster1_outNeighs[member].append(IDtoBigNode[outNeigh])
#                 else:
#                     cluster1_outNeighs[member].append(outNeigh)
#             # if inNeighs are in a cluster, set inNeighs to that cluster
#             for inNeigh in graphInfo.inNeigh[member]:
#                 if inNeigh in IDtoBigNode:
#                     cluster1_inNeighs[member].append(IDtoBigNode[inNeigh])
#                 else:
#                     cluster1_inNeighs[member].append(inNeigh)
bigNodeIDToMembers = {}
relevantClusters = list(filter(lambda pair:len(pair[1])>8, MergeIDToMembers2.items()))
# cluster1_inNeighs = {i:[j for j in graphInfo.inNeigh[i]] for i in graphInfo.inNeigh}
# cluster1_outNeighs = {i:[j for j in graphInfo.outNeigh[i]] for i in graphInfo.outNeigh}
#sanity check
# for mergeID, members in relevantClusters:
#     for mem in members:
#         assert(mem in cluster1_inNeighs)
#         assert(mem in cluster1_outNeighs)
# for mergeID, members in relevantClusters:
#     repres = members.pop()
#     bigNodeIDToMembers[repres] = members
#     #update any outNeighs
#     for mem in members:
#         if mem==5:
#             print("mem found")
#         anyOut = False
#         for outNeigh in graphInfo.outNeigh[mem]:
#             if not outNeigh in members:
#                 anyOut = True
#                 if outNeigh==5:
#                     print("outNeigh")
#                 if mem in cluster1_inNeighs[outNeigh]:
#                     cluster1_inNeighs[outNeigh].remove(mem)
#                 cluster1_inNeighs[outNeigh].append(repres)
#                 cluster1_outNeighs[repres].append(outNeigh)
#         if not mem in cluster1_outNeighs:
#             print("not in outneigh",mem)
#         anyIn = False
#         for inNeigh in graphInfo.inNeigh[mem]:
#             if not inNeigh in members:
#                 anyIn = True
#                 if inNeigh==5:
#                     print("inNeigh")
#                 if mem in cluster1_outNeighs[inNeigh]:
#                     cluster1_outNeighs[inNeigh].remove(mem)
#                 cluster1_outNeighs[inNeigh].append(repres)
#                 cluster1_inNeighs[repres].append(inNeigh)
#         if not mem in cluster1_inNeighs:
#             print("not in inneighs",mem)
#         if anyOut:
#             del cluster1_outNeighs[mem]
#         if anyIn:
#             del cluster1_inNeighs[mem]
    #remove all nodes from in/outNeigh, except representative node
# cluster1_inNeighs = {i:set([j for j in graphInfo.inNeigh[i]]) for i in graphInfo.inNeigh}
# cluster1_outNeighs = {i:set([j for j in graphInfo.outNeigh[i]]) for i in graphInfo.outNeigh}
# for mergeID, members in relevantClusters:
#     repres = members.pop()
#     bigNodeIDToMembers[repres] = members
#     #update any outNeighs
#     connected = False
#     for mem in members:
#         if mem == 581:
#             print("BUG!2")
#         toRemove = set()
#         for outNeigh in cluster1_outNeighs[mem]:
#             if not outNeigh in members:
#                 connected = True
#                 cluster1_inNeighs[outNeigh].add(repres)
#                 cluster1_outNeighs[repres].add(outNeigh)
#                 cluster1_inNeighs[outNeigh].remove(mem)
#                 toRemove.add(outNeigh)
#         for tr in toRemove:
#             cluster1_outNeighs[mem].remove(tr)
#         toRemove = set()
#         for inNeigh in cluster1_inNeighs[mem]:
#             if not inNeigh in members:
#                 connected = True
#                 cluster1_outNeighs[inNeigh].add(repres)
#                 cluster1_inNeighs[repres].add(inNeigh)
#                 cluster1_outNeighs[inNeigh].remove(mem)
#                 toRemove.add(inNeigh)
#         for tr in toRemove:
#             cluster1_inNeighs[mem].remove(tr)
#     for mem in members:
#         if mem == 581:
#             print("BUG!")
#         del cluster1_inNeighs[mem]
#         del cluster1_outNeighs[mem]
#     if not connected:
#         del cluster1_inNeighs[repres]
#         del cluster1_outNeighs[repres]        
    

# cluster1_inNeighs = {i:set([j for j in graphInfo.inNeigh[i]]) for i in graphInfo.inNeigh}
# cluster1_outNeighs = {i:set([j for j in graphInfo.outNeigh[i]]) for i in graphInfo.outNeigh}
from collections import defaultdict
cluster1_inNeighs = defaultdict(set)
cluster1_outNeighs = defaultdict(set)
for ID in graphInfo.inNeigh:
    cluster1_inNeighs[ID] = set([j for j in graphInfo.inNeigh[ID]])
for ID in graphInfo.outNeigh:
    cluster1_outNeighs[ID] = set([j for j in graphInfo.outNeigh[ID]])

repres = 70000
for mergeID, members in relevantClusters:
    repres = repres+1
    bigNodeIDToMembers[repres] = members
    #update any outNeighs
    connected = False
    for mem in members:
        if mem == 581:
            print("BUG!2")
        toRemove = set()
        for outNeigh in cluster1_outNeighs[mem]:
            if not outNeigh in members:
                connected = True
                cluster1_inNeighs[outNeigh].add(repres)
                cluster1_outNeighs[repres].add(outNeigh)
                cluster1_inNeighs[outNeigh].remove(mem)
                toRemove.add(outNeigh)
        for tr in toRemove:
            cluster1_outNeighs[mem].remove(tr)
        toRemove = set()
        for inNeigh in cluster1_inNeighs[mem]:
            if not inNeigh in members:
                connected = True
                cluster1_outNeighs[inNeigh].add(repres)
                cluster1_inNeighs[repres].add(inNeigh)
                cluster1_outNeighs[inNeigh].remove(mem)
                toRemove.add(inNeigh)
        for tr in toRemove:
            cluster1_inNeighs[mem].remove(tr)
    for mem in members:
        if mem == 581:
            print("BUG!")
        del cluster1_inNeighs[mem]
        del cluster1_outNeighs[mem]
    # if not connected:
    #     del cluster1_inNeighs[repres]
    #     del cluster1_outNeighs[repres]


#sanity check:
for bgid in bigNodeIDToMembers:
    for mem in bigNodeIDToMembers[bgid]:
        if mem in cluster1_inNeighs:
            assert(False)
        if mem in cluster1_outNeighs:
            assert(False)

# 1b: export bigNodeIDToMembers:
f = open("bigNodeIDToMembers_tmp","w")
for bigNodeID, members in bigNodeIDToMembers.items():
    f.write(str(bigNodeID)+" "+''.join([str(m)+" " for m in members])+"\n")
f.close()

#sanity check:
for ID,inNeighs in graphInfo.inNeigh.items():
    for inNeigh in inNeighs:
        if not ID in graphInfo.outNeigh[inNeigh]:
            assert(False)
validSet = set(graphInfo.validNodes)
for ID,outNeighs in graphInfo.outNeigh.items():
    if ID in validSet:
        for outNeigh in outNeighs:
            if not ID in graphInfo.inNeigh[outNeigh] and outNeigh in validSet:
                print(ID,outNeighs)
                # assert(False)
for ID,inNeighs in cluster1_inNeighs.items():
    for inNeigh in inNeighs:
        if not ID in cluster1_outNeighs[inNeigh]:
            assert(False)
for ID,outNeighs in cluster1_outNeighs.items():
    for outNeigh in outNeighs:
        if not ID in cluster1_inNeighs[outNeigh]:
            assert(False)

# 2: export this graph to a dot file
dumpToDotFileNeighs(cluster1_inNeighs, cluster1_outNeighs, "42023_dump_bignode_graph.dot")
dumpToDotFileNeighs(graphInfo.inNeigh, graphInfo.outNeigh, "42023_dump_unmod_graph.dot")


#DEBUG 42023: not a bug!
cluster1_inNeighs = {i:[j for j in graphInfo.inNeigh[i]] for i in graphInfo.inNeigh}
cluster1_outNeighs = {i:[j for j in graphInfo.outNeigh[i]] for i in graphInfo.outNeigh}
for mergeID, members in relevantClusters:
    repres = members.pop()
    bigNodeIDToMembers[repres] = members
    #update any outNeighs
    for mem in members:
        for outNeigh in graphInfo.outNeigh[mem]:
            if not outNeigh in members:
                for inNeigh in graphInfo.inNeigh[mem]:
                    if not inNeigh in members:
                        for i in set(graphInfo.outNeigh[mem]):
                            if i in set(graphInfo.inNeigh[mem]):
                                print("uhoh1")