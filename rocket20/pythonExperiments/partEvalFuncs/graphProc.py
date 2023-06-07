from collections import defaultdict
from partEvalFuncs.unionfind import *

def getNoInNoOut(graphInfo):
    noIn = [i[0] for i in filter(lambda x: len(x[1])==0, graphInfo.inNeigh.items())]
    noOut = [i[0] for i in filter(lambda x: len(x[1])==0, graphInfo.outNeigh.items())]
    return noIn, noOut

# metric 1: cluster input and output nodes
# FOR CLUSTER that start at the course, THIS
# DOESN'T RECORD THEM!
def getBoundaries(outNeigh, IDToMergeID, noIn, noOut):
    clusterToInputIDs = defaultdict(list)
    clusterToOutputIDs = defaultdict(list)
    for n in outNeigh:
        for neigh in outNeigh[n]:
            if neigh!=None and IDToMergeID[n] != IDToMergeID[neigh]:
                clusterToOutputIDs[IDToMergeID[n]].append(n)
                clusterToInputIDs[IDToMergeID[neigh]].append(neigh)
    for n in noIn:
        if neigh!=None and IDToMergeID[n] != IDToMergeID[neigh]:
            clusterToInputIDs[IDToMergeID[n]].append(n)
    for n in noOut:
        if neigh!=None and IDToMergeID[n] != IDToMergeID[neigh]:
            clusterToOutputIDs[IDToMergeID[n]].append(n)
    return clusterToOutputIDs, clusterToInputIDs


#021323
#find all of the inputs that influence any given node
def find_sources(graph, inGraph):
    result = defaultdict(list)
    queue = []
    for node in inGraph:
        if not inGraph[node]:
            queue.append(node)
    while queue:
        #print("to pop:",len(queue))
        srcNode = queue.pop(0)
        result[srcNode].append(srcNode)
        front = [srcNode]
        visited = [0 for _ in graph]
        while(front):
            toProc = front.pop(0)
            #print("front:",front)
            for child in graph[toProc]:
                #print("\tchild:",child)
                if not visited[child]:
                    #print("\t\tadded:",child)
                    visited[child] = 1
                    result[child].append(srcNode)
                    #print(result[child])
                    front.append(child)
    return result


#040723
#find all of the non-Sig inputs that influence any given node
def find_sources_sig(graph, noInSig):
    result = defaultdict(list)
    queue = noInSig
    print(noInSig)
    while queue:
        #print("to pop:",len(queue))
        srcNode = queue.pop(0)
        result[srcNode].append(srcNode)
        front = [srcNode]
        visited = [0 for _ in graph]
        while(front):
            toProc = front.pop(0)
            #print("front:",front)
            for child in graph[toProc]:
                #print("\tchild:",child)
                if not visited[child]:
                    #print("\t\tadded:",child)
                    visited[child] = 1
                    result[child].append(srcNode)
                    #print(result[child])
                    front.append(child)
    return result

def allSharedSrcs(graphInfo, noIn, nodeToSrcs):
    IDToMergeID2 = {}
    MergeIDToMembers2 = []
    visited = [0 for i in range(len(graphInfo.inNeigh))]
    front = noIn
    mergeID = 0
    debugMembersLen = []
    while len(front)>0:
        #print(sum(visited))
        toProc = front.pop(0)
        sources = nodeToSrcs[toProc]
        members = []
        minifront = [toProc]
        #print("processing:",toProc,"srcNodes = ",sources)
        while len(minifront)>0:
            miniToProc = minifront.pop(0)
            members.append(miniToProc)
            #print("\t children = ",graphInfo.outNeigh[miniToProc])
            for n in graphInfo.outNeigh[miniToProc]:
                #if n shares a set of srcs with n AND if n has not been visited yet
                #print("\tchild:",n," has srcs:",nodeToSrcs[n])
                #sanity check here
                # if len(graphInfo.inNeigh[n])==1 and visited[n]==0:
                #     if not(set(nodeToSrcs[n])==set(sources)):
                #         print("ERROR")
                #         print(nodeToSrcs[n],nodeToSrcs[toProc])
                #         exit(111)
                # if set(nodeToSrcs[n])==set(sources) and visited[n]==0:
                if len(graphInfo.inNeigh[n])==1 and visited[n]==0:
                    minifront.append(n)
                    visited[n] = 1
                else:
                    # print("correct",mergeID)
                    if not visited[n]:
                        assert(not n in front)
                        visited[n] = 1
                        front.append(n)
        debugMembersLen.append(len(members))
        MergeIDToMembers2.append(members)
        assert(len(MergeIDToMembers2)==mergeID+1)
        for m in members:
            IDToMergeID2[m] = mergeID
        mergeID += 1
    return IDToMergeID2, MergeIDToMembers2