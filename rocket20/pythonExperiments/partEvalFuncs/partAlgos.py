from partEvalFuncs.graphProc import *

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