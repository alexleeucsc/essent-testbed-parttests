from partEvalFuncs.graphProc import *

class graphInfo:
    def __init__(self):
        self.IDToMergeID = []
        self.MergeIDToMembers = {}
        self.nameToID = {}
        self.IDToName = []
        self.IDToStmt = {}
        self.validNodes = []
        self.inNeigh = {}
        self.outNeigh = {}
        self.IDToWidth = []

def readGraph(graphOutputLocation):
    graphObj = graphInfo()
    graphFile = open(graphOutputLocation,"r")
    mode = None
    for line in graphFile:
        line = line.strip("\n")
        # print(line)
        if line.lower() in ["idtomergeid","mergeidtomembers","nametoid","idtoname","idtostmt","validnodes","inneigh","outneigh",
                            "priorinneigh","prioroutneigh","idtonodewidth"]:
            mode = line.lower()
            continue
        if mode=="idtomergeid":
            graphObj.IDToMergeID = line.split("<1>")[:-1]
            graphObj.IDToMergeID = [int(i) for i in graphObj.IDToMergeID]
            mode = "none"
            continue
        if mode=="idtonodewidth":
            lineComp = ""
            while("idToNodeWidthEND" not in line):
                lineComp += line
                line = next(graphFile)
            graphObj.IDToWidth = line.split("<1>")[:-1]
            graphObj.IDToWidth = [int(i) for i in graphObj.IDToWidth]
            mode = "none"
            continue
        if mode=="mergeidtomembers":
            split1 = line.split("<1>")[:-1]
            for i in range(len(split1)):
                nxt = []
                mergeID, members = split1[i].split("<2>")
                members = members.split("<3>")[:-1]
                if len(members)>0:
                    nxt = [int(i) for i in members]
                graphObj.MergeIDToMembers[int(mergeID)] = nxt
            mode = "none"
            mode = "none"
            continue
        if mode=="nametoid":
            split1 = line.split("<1>")[:-1]
            graphObj.nameToID = {i.split("<2>")[0]:(i.split("<2>")[1]) for i in split1}
            mode = "none"
            continue
        if mode=="idtoname":
            graphObj.IDToName = line.split("<1>")
            mode = "none"
            continue
        if mode=="idtostmt":
            graphObj.IDToStmt = line.split("<1>")
            mode = "none"
            continue
        if mode=="validnodes":
            graphObj.validNodes = line.split("<1>")
            graphObj.validNodes = [int(i) for i in graphObj.validNodes[:-1]]
            mode = "none"
            continue
        if mode=="inneigh" or mode=="priorinneigh":
            print("inneigh")
            split1 = line.split("<1>")[:-1]
            for i in range(len(split1)):
                nxt = []
                neigh = split1[i].split("<2>")[:-1]
                if len(neigh)>0:
                    nxt = [int(i) for i in neigh]
                graphObj.inNeigh[i] = nxt
            mode = "none"
            continue
        if mode=="outneigh" or mode=="prioroutneigh":
            split1 = line.split("<1>")[:-1]
            for i in range(len(split1)):
                nxt = []
                neigh = split1[i].split("<2>")[:-1]
                if len(neigh)>0:
                    nxt = [int(i) for i in neigh]
                graphObj.outNeigh[i] = nxt
            mode = "none"
            continue
    graphFile.close()
    return graphObj

def getIDToSigFromHarness(graphInfo, harnessFilePath, cycleLimit = 10000):
    harnessFile = open(harnessFilePath,"r")
    sigToID = {}
    nameToIDFix = dict([(i[0].replace(".","$"),i[1]) for i in graphInfo.nameToID.items()])
    cycle = 0
    for line in harnessFile:
        if "SIGcount" in line:
            if "[" in line and "]" in line and "+=" in line and "!=" in line:
                if line[line.index("+=")+3:line.index("!=")-1] in nameToIDFix:
                    sigToID[int(line[line.index("[")+1:line.index("]")])] = int(nameToIDFix[line[line.index("+=")+3:line.index("!=")-1]])
                else:
                    print(line[line.index("+=")+3:line.index("!=")-1])
            else:
                print(line)
    IDToSig = {}
    for sig in sigToID:
        IDToSig[sigToID[sig]] = sig
    return sigToID, IDToSig

def getActivationList(activationFilePath, cycleLimit):
    activationList = []
    activationFile = open(activationFilePath,"r")
    debugIdx = 0
    prevList = next(activationFile)
    prevList = ''.join(prevList.split("\t")[:-1])
    _ = next(activationFile)
    activationList.append(prevList)
    for line in activationFile:
        lineList = line.split("\t")[:-1]
        changeLine = ['0']*len(lineList)
        #print("line lens:",len(prevList),len(lineList))
        for i in range(len(prevList)):
            if prevList[i]!=lineList[i]:
                if not (int(prevList[i])+1 == int(lineList[i])):
                    print(prevList[i],lineList[i])
                    exit(111)
                changeLine[i]='1'
        changeLine = ''.join(changeLine)
        activationList.append(changeLine)
        debugIdx += 1
        prevList = lineList
        _ = next(activationFile)
        if debugIdx == cycleLimit:
            break
    return activationList

