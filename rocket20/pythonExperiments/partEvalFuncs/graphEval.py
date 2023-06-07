from partEvalFuncs.graphProc import *
import time
cycleLimit = 2000

def getInputActivationCount(cluster2InActPerc, activations, IDToMergeID, MergeIDToMembers, IDToSig, clusterToInputIDs, debugIdx, graphInfo, partActiveDebug=False):
    total,active,totalReqAct = 0,0,0
    # if debugIdx>10:
    #     print("BEGIN DEBUG LOG")
    partActLens = []
    for cluster in MergeIDToMembers:
        # print("\tcheck next cluster")
        clusterActive = False
        allLegalIns = list(filter(lambda x:x in IDToSig, clusterToInputIDs[cluster]))
        # debug note: this was from mistake when I thought inputs might be wrong
        # this os okay, actually
        # if not (all(map(lambda x:x=='0', allLegalIns)) or all(map(lambda x:x=='1', allLegalIns))):
        #     print("ERROR: not all sigs in cluster are eq?")
        #     print([IDToSig[i] for i in allLegalIns])
        for inputID in clusterToInputIDs[cluster]:
            if inputID in IDToSig:
                actidx = IDToSig[inputID]
                if activations[actidx]=='1':
                    clusterActive = True
        if clusterActive:
            # if debugIdx > 10:
            #     print("cid:", cluster, "active:", len( [i for i in filter(lambda x:x in IDToSig, MergeIDToMembers[cluster])] ))
            #POSSIBLE BUG: not all ID have a corresponding sig...
            SigInCluster = list(map(lambda ID2:IDToSig[ID2], filter(lambda ID:ID in IDToSig, MergeIDToMembers[cluster])))
            # SigInCluster = list(map(lambda ID2:IDToSig[ID2], filter(lambda Sig:Sig in sigToID, filter(lambda ID:ID in IDToSig, MergeIDToMembers[cluster]))))
            cluster2InActPerc[cluster] = len( SigInCluster )
            if partActiveDebug:
                #debugging: countUneq = number of signals not equal to signal 0; if ANY, report cluster
                try:
                    countUneq = len(list(filter( lambda x:activations[x]!=activations[SigInCluster[0]], SigInCluster ))) #bug; activation[0] is first activation in all list, need activations[SigInCluster[0]]
                except IndexError:
                    print("error:")
                    print("lens: sig, act")
                    print(SigInCluster)
                    print(len(activations))
                    exit(111)
                #partActLens.append([countUneq,len(SigInCluster)])
                clusterInputSigs = list(map(lambda ID2:IDToSig[ID2], filter(lambda ID:ID in IDToSig, clusterToInputIDs[cluster])))
                #
                if countUneq > 0:
                    #BUG: partActLens.append([countUneq,len(SigInCluster)]) countUneq is the number of activations not equal to the FIRST sig in cluster; not total activations!
                    #BUG: while reading error log, remember that partActLen here == len of clusters that are onnly partially active (not all on or off)
                    partActLens.append([sum(list(map( lambda x:int(activations[x]), SigInCluster ))),len(SigInCluster), cluster])
                    #1: record number of nodes that are 0 when it's parents are 1
                    # print("\tcluster", cluster, 
                    # "SigInCluster", SigInCluster,
                    # "clusterInputSigs", clusterInputSigs,
                    # "activations",list(map(lambda sig:activations[sig], SigInCluster)))
                    # print("\n")
        #print("\n")
        if debugIdx > 10 and partActiveDebug:
            memCount = len( [i for i in filter(lambda x:x in IDToSig, MergeIDToMembers[cluster])] )
            total = total + memCount
            totalReqAct = totalReqAct + sum( [int(activations[IDToSig[idx]]) for idx in filter(lambda x:x in IDToSig, MergeIDToMembers[cluster])] )
            if clusterActive:
                active = active + memCount
            if (not clusterActive) and totalReqAct > 0 and False:
                print("ERROR: during cluster where no input was on (not clusterActive), a member was on")
                print("inputs:")
                for inputID in clusterToInputIDs[cluster]:
                    print("input ID:",inputID)
                    if inputID in IDToSig:
                        actidx = IDToSig[inputID]
                        print("activation:",activations[actidx])
                for member in filter(lambda x:x in IDToSig, MergeIDToMembers[cluster]):
                    print("memebr:", member,end=" ")
                    print("inNeigh", graphInfo.inNeigh[member], end = " ")
                    print("sig@", [IDToSig[i] if i in IDToSig else "MISSING" for i in graphInfo.inNeigh[member]], end = " ")
                    print("outNeigh",graphInfo.outNeigh[member], end = " ")
                    print("sig@", [IDToSig[i] if i in IDToSig else "MISSING" for i in graphInfo.outNeigh[member]], end = " ")
                    print(int(activations[IDToSig[member]]))
                #     for inNeigh in graphInfo.inNeigh[member]:
                #         print("parent activation:")
                #         print(int(activations[IDToSig[inNeigh]]))
                exit(111)
    if partActiveDebug:    
        print("lens of partially active clusters")
        partActLens.sort(key = lambda x:x[1]-x[0])
        totalNonReqAct = 0
        for actCount, partitionLen, cluster in partActLens:
            print("actCount",actCount,"partitionLen",partitionLen,"activation ratio",actCount/partitionLen, "cluster:", cluster)
            totalNonReqAct += partitionLen-actCount
            print("\tcurrent totalNonReqAct:",totalNonReqAct)
        print("sum partitionLen:", sum([i[1] for i in partActLens]))
        print("total non required:", totalNonReqAct)
        print("\n\n")
        if(total < totalReqAct):
            print("ERROR")
            exit(111)
                # print("cid:", cluster)
                # print("memberCount:", memCount)
                # print("total:",total,"vs active:",active)

def evaluateClusters(graphInfo, IDToMergeID, MergeIDToMembers, activationList, sigToID, IDToSig):
    cluster2activations = {}
    cluster2InputActivations = {}
    #cluster2InActPerc = {} //021723: RECORD THIS BUG!
    debugidx = 0
    #get cluster inputs and sources
    noIn, noOut = getNoInNoOut(graphInfo)
    clusterToOutputIDs, clusterToInputIDs = getBoundaries(graphInfo.outNeigh, IDToMergeID, noIn, noOut)
    #sanity check: all inputIDs should be in their corresponding cluster
    for cluster in clusterToInputIDs:
        for inputID in clusterToInputIDs[cluster]:
            assert(inputID in MergeIDToMembers[cluster])
            assert(inputID in graphInfo.MergeIDToMembers[cluster])
    print("sanity checked 1")
    totalSigs = 0
    totalAct = 0
    totalActReq = 0
    #BUG 050923: activation List's last item is '': NOT the last item from each item from the activationList
    activationList = activationList[:-1]
    for activationString in activationList:
        actList = list(activationString)
        cluster2InActPerc = {}
        getInputActivationCount(cluster2InActPerc, actList, IDToMergeID, MergeIDToMembers, IDToSig, clusterToInputIDs, debugidx, graphInfo)
        # check sum of all cluster sizes
        actPerc = sum(cluster2InActPerc.values()) / len(actList)
        reqActPerc = sum([int(actList[i]) for i in filter(lambda x:x in sigToID, list(range(len(actList))))]) / len(actList)
        print("total sigs in active clusters",sum(cluster2InActPerc.values()), "signals==1 count", sum([int(actList[i]) for i in filter(lambda x:x in sigToID, list(range(len(actList))))]))
        print(actPerc,reqActPerc,actPerc-reqActPerc)
        # check sum of only n largest clusters 
        # topNClusters = sorted(graphInfo.MergeIDToMembers.keys(), keys=lambda x:x[1])
        # topNClusters = [i[0] for i in sorted(graphInfo.MergeIDToMembers.items(), key=lambda x:len(x[1]))][-3:-1]
        # actPerc_topn = sum(cluster2InActPerc[i] for i in topNClusters) / len(actList)
        # nodesInTopNClusters = sum([graphInfo.MergeIDToMembers[i] for i in topNClusters], start=[])
        # reqActPerc_topn = sum([int(actList[i]) for i in filter(lambda x:x in sigToID, nodesInTopNClusters)]) / len(actList)
        # print("in top n clusters, the activation for this cycle is:")
        # print(actPerc_topn,reqActPerc_topn,actPerc_topn-reqActPerc_topn)
        totalSigs = totalSigs + len(actList)
        totalAct = totalAct + sum(cluster2InActPerc.values())
        totalActReq = totalActReq + sum([int(actList[i]) for i in filter(lambda x:x in sigToID, list(range(len(actList))))])
        totalActPerc = totalAct / totalSigs
        totalActReqPerc = totalActReq / totalSigs
        print("total")
        print(totalAct, totalActReq)
        print(totalActPerc,totalActReqPerc,totalActPerc-totalActReqPerc)        
        print("\n")
        debugidx += 1
        if debugidx == cycleLimit:
            break

def evaluateClusters_printSrcInfo(graphInfo, IDToMergeID, MergeIDToMembers, activationList, sigToID, IDToSig, IDToSrc):
    cluster2activations = {}
    cluster2InputActivations = {}
    #cluster2InActPerc = {} //021723: RECORD THIS BUG!
    debugidx = 0
    #get cluster inputs and sources
    noIn, noOut = getNoInNoOut(graphInfo)
    clusterToOutputIDs, clusterToInputIDs = getBoundaries(graphInfo.outNeigh, IDToMergeID, noIn, noOut)
    #sanity check: all inputIDs should be in their corresponding cluster
    for cluster in clusterToInputIDs:
        for inputID in clusterToInputIDs[cluster]:
            assert(inputID in MergeIDToMembers[cluster])
            assert(inputID in graphInfo.MergeIDToMembers[cluster])
    print("sanity checked 1")
    totalSigs = 0
    totalAct = 0
    totalActReq = 0
    for activationString in activationList:
        actList = list(activationString)[:-1]
        cluster2InActPerc = {}
        getInputActivationCount_printSrcInfo(cluster2InActPerc, actList, IDToMergeID, MergeIDToMembers, IDToSig, clusterToInputIDs, debugidx, graphInfo, IDToSrc)
        # check sum of all cluster sizes
        actPerc = sum(cluster2InActPerc.values()) / len(actList)
        reqActPerc = sum([int(actList[i]) for i in filter(lambda x:x in sigToID, list(range(len(actList))))]) / len(actList)
        print("total sigs in active clusters",sum(cluster2InActPerc.values()), "signals==1 count", sum([int(actList[i]) for i in filter(lambda x:x in sigToID, list(range(len(actList))))]))
        print(actPerc,reqActPerc,actPerc-reqActPerc)
        # check sum of only n largest clusters 
        # topNClusters = sorted(graphInfo.MergeIDToMembers.keys(), keys=lambda x:x[1])
        # topNClusters = [i[0] for i in sorted(graphInfo.MergeIDToMembers.items(), key=lambda x:len(x[1]))][-3:-1]
        # actPerc_topn = sum(cluster2InActPerc[i] for i in topNClusters) / len(actList)
        # nodesInTopNClusters = sum([graphInfo.MergeIDToMembers[i] for i in topNClusters], start=[])
        # reqActPerc_topn = sum([int(actList[i]) for i in filter(lambda x:x in sigToID, nodesInTopNClusters)]) / len(actList)
        # print("in top n clusters, the activation for this cycle is:")
        # print(actPerc_topn,reqActPerc_topn,actPerc_topn-reqActPerc_topn)
        totalSigs = totalSigs + len(actList)
        totalAct = totalAct + sum(cluster2InActPerc.values())
        totalActReq = totalActReq + sum([int(actList[i]) for i in filter(lambda x:x in sigToID, list(range(len(actList))))])
        totalActPerc = totalAct / totalSigs
        totalActReqPerc = totalActReq / totalSigs
        print("total")
        print(totalAct, totalActReq)
        print(totalActPerc,totalActReqPerc,totalActPerc-totalActReqPerc)        
        print("\n")
        debugidx += 1
        if debugidx == cycleLimit:
            break

def evaluatePartialClusters(graphInfo, IDToMergeID, MergeIDToMembers, activationList, sigToID, IDToSig):
    # sanity check: IDToMergeID must have same clusters as MergeIDToMembers
    cluster2activations = {}
    cluster2InputActivations = {}
    #cluster2InActPerc = {} //021723: RECORD THIS BUG!
    debugidx = 0
    #get cluster inputs and sources
    noIn, noOut = getNoInNoOut(graphInfo)
    clusterToOutputIDs, clusterToInputIDs = getBoundaries(graphInfo.outNeigh, IDToMergeID, noIn, noOut)
    #sanity check: all inputIDs should be in their corresponding cluster
    # for cluster in clusterToInputIDs:
    #     for inputID in clusterToInputIDs[cluster]:
    #         assert(inputID in MergeIDToMembers[cluster])
    #         assert(inputID in graphInfo.MergeIDToMembers[cluster])
    print("sanity checked 1")
    totalSigs = 0
    totalAct = 0
    totalActReq = 0
    total_in_partial = 0
    totalMemberIDs = sum(MergeIDToMembers.values(),start=[])
    for activationString in activationList:
        actList = list(activationString)[:-1]
        cluster2InActPerc = {}
        # MergeIDToMembers_bigOnlyList = list(filter(lambda x:len(x[1])>8, MergeIDToMembers.items()))
        # MergeIDToMembers_bigOnly = {i:MergeIDToMembers[i] for i in MergeIDToMembers_bigOnlyList}
        #print("debug@042823: total clust:", len(MergeIDToMembers), [len(members) for ID,members in MergeIDToMembers.items()])
        getInputActivationCount(cluster2InActPerc, actList, IDToMergeID, MergeIDToMembers, IDToSig, clusterToInputIDs, debugidx, graphInfo)
        # check sum of all cluster sizes
        actPerc = sum(cluster2InActPerc.values()) / sum([len(membs) for ID,membs in MergeIDToMembers.items()])
        #DEBUG 042823: assert that we are indeed only checking big clusters:
        #print("debug@042823: active,total=", sum(cluster2InActPerc.values()), sum([len(membs) for ID,membs in MergeIDToMembers.items()]))
        #print("debug@042823: all activities in all clusters=", sum([int(i) for i in actList]))
        #reqActPerc = sum([int(actList[i]) for i in filter(lambda x:x in sigToID, list(range(len(actList))))]) / len(actList)
        #reqAct for given MergeIDs only
        #print("debug@042823: max sig in id to sig, len of actList:", max(IDToSig.values()), len(actList))
        assert(max(IDToSig.values()) <= len(actList))
        # totalMemberIDs = sum(MergeIDToMembers.values(),start=[])
        totalRelevantMemberSigs = list(map(lambda ID:int(IDToSig[ID]), filter(lambda ID:ID in IDToSig, totalMemberIDs)))
        reqActPerc = sum([int(actList[i]) for i in totalRelevantMemberSigs]) / len(totalRelevantMemberSigs)
        print("number of members with signal vs without:",len(totalRelevantMemberSigs),"vs",sum([len(membs) for ID,membs in MergeIDToMembers.items()]))
        print("total sigs in active clusters",sum(cluster2InActPerc.values()), "signals==1 count", sum([int(actList[i]) for i in filter(lambda x:x in sigToID, list(range(len(actList))))]))
        print(actPerc,reqActPerc,actPerc-reqActPerc)
        # check sum of only n largest clusters 
        # topNClusters = sorted(graphInfo.MergeIDToMembers.keys(), keys=lambda x:x[1])
        # topNClusters = [i[0] for i in sorted(graphInfo.MergeIDToMembers.items(), key=lambda x:len(x[1]))][-3:-1]
        # actPerc_topn = sum(cluster2InActPerc[i] for i in topNClusters) / len(actList)
        # nodesInTopNClusters = sum([graphInfo.MergeIDToMembers[i] for i in topNClusters], start=[])
        # reqActPerc_topn = sum([int(actList[i]) for i in filter(lambda x:x in sigToID, nodesInTopNClusters)]) / len(actList)
        # print("in top n clusters, the activation for this cycle is:")
        # print(actPerc_topn,reqActPerc_topn,actPerc_topn-reqActPerc_topn)
        totalSigs = totalSigs + len(actList)
        totalAct = totalAct + sum(cluster2InActPerc.values())
        totalActReq = totalActReq + sum([int(actList[i]) for i in filter(lambda x:x in sigToID, list(range(len(actList))))])
        totalActPerc = totalAct / totalSigs
        totalActReqPerc = totalActReq / totalSigs
        print("total")
        print(totalAct, totalActReq)
        print(totalActPerc,totalActReqPerc,totalActPerc-totalActReqPerc)        
        print("\n")
        debugidx += 1
        if debugidx == cycleLimit:
            break



def evaluateSpecNodes(graphInfo, IDToMergeID, MergeIDToMembers, specNodes, activationList, sigToID, IDToSig):
    cluster2activations = {}
    cluster2InputActivations = {}
    #cluster2InActPerc = {} //021723: RECORD THIS BUG!
    debugidx = 0
    #get cluster inputs and sources
    noIn, noOut = getNoInNoOut(graphInfo)
    clusterToOutputIDs, clusterToInputIDs = getBoundaries(graphInfo.outNeigh, IDToMergeID, noIn, noOut)
    print("sanity checked 1")
    totalSigs = 0
    totalAct = 0
    totalActReq = 0
    total_in_partial = 0
    #MergeIDsToCheck_List are the mergeIDs of clusters that contain the specNodes to check
    MergeIDsToCheck_List = list(set(IDToMergeID[ID] for ID in specNodes))
    MergeIDToMembers_toCheck = {mergeID:MergeIDToMembers[mergeID] for mergeID in MergeIDsToCheck_List}
    for activationString in activationList:
        #051123 BUG : changed getActivaitons so that chopping off "\n" happens there
        actList = list(activationString)
        cluster2InActPerc = {}
        print("debug@042823: total clust:", len(MergeIDToMembers_toCheck), [len(members) for ID,members in MergeIDToMembers_toCheck.items()])
        getInputActivationCount(cluster2InActPerc, actList, IDToMergeID, MergeIDToMembers_toCheck, IDToSig, clusterToInputIDs, debugidx, graphInfo)
        # check sum of all cluster sizes
        # actPerc = sum(cluster2InActPerc.values()) / sum([len(membs) for ID,membs in MergeIDToMembers_toCheck.items()])
        totalActiveSpecNodes, totalActiveSpecNodesInClust = 0, 0
        # Get totalActiveSpecNodesInClust; note that  returns a dict:
        # {cluster ID of active clusters : len of cluster}
        # if the cluster ID is in cluster2InActPerc, then it should be all active
        for n in specNodes:
            relevantMergeID = IDToMergeID[n]
            if relevantMergeID in cluster2InActPerc:
                totalActiveSpecNodesInClust += 1
        for n in specNodes:
            if n in IDToSig:
                totalActiveSpecNodes += 1 if actList[IDToSig[n]]=='1' else 0
        actPerc = totalActiveSpecNodesInClust / len(specNodes)
        reqActPerc = totalActiveSpecNodes / len(specNodes)
        print(actPerc,reqActPerc,actPerc-reqActPerc)
        totalSigs = totalSigs + len(specNodes)
        totalAct = totalAct + totalActiveSpecNodesInClust
        totalActReq = totalActReq + totalActiveSpecNodes
        totalActPerc = totalAct / totalSigs
        totalActReqPerc = totalActReq / totalSigs
        print("total")
        print(totalAct, totalActReq)
        print(totalActPerc,totalActReqPerc,totalActPerc-totalActReqPerc)        
        print("\n")
        debugidx += 1
        if debugidx == cycleLimit:
            break