from partEvalFuncs.graphProc import *

def getStopActivations11(graphInfo, activationList):
    #> find node types that tend to remove activation
    stopActivation = [[0 for j in i] for i in outNeigh.values()]
    actCount = [0 for i in outNeigh]
    for act in activationList:
        for n in range(len(outNeigh)):
            if n in IDToSig:
                for neighIdx in range(len(outNeigh[n])):
                    neigh = outNeigh[n][neighIdx]
                    if neigh in IDToSig:
                        if act[IDToSig[n]]=='1':
                            actCount[n] += 1
                        if act[IDToSig[n]]=='1' and act[IDToSig[neigh]]=='0':
                            stopActivation[n][neighIdx] += 1
    #stopActivation is a copy of outNeigh such that the item here:
    #[[0,13,4...]...[(list 5) 3,4,5... ]...]
    #item 0 in list 5 means that the node with ID=5 is activated but kills the signal to child 0 3 times
    for n in range(len(stopActivation)):
        if n in IDToSig:
            stopAct = stopActivation[n]
            for child in stopAct:
                if child>int(actCount[n]/2):
                    print(n,child)
                else:
                    print("\t",n,child)
    return stopActivation