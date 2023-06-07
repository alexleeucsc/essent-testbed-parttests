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

#run essent
#../essent/utils/bin/essent --part-cutoff 20 --activity-signal --graphDump 531_rocket20_k20_graphDump --essent-log-level debug ../rocket18/freechips.rocketchip.system.DefaultConfig.fir
#then eval graph

from collections import defaultdict
from sklearn import decomposition
from sklearn.cluster import KMeans
from sklearn.cluster import MiniBatchKMeans
import numpy as np

import sys
print (sys.argv)
import time
#get orig info
import subprocess

for clusterCutoff in range(5,30):
    file_ = open("531_rocket20_k"+str(clusterCutoff)+"_graphDumpStatistics.log", "w")
    p = subprocess.Popen(["../../../../essent/utils/bin/essent", "--part-cutoff", str(clusterCutoff), "--activity-signal", "--graphDump", "531_rocket20_k"+str(clusterCutoff)+"_graphDump","--essent-log-level","debug","../../../../rocket20/freechips.rocketchip.system.DefaultConfig.fir"], stdout=file_)
    p.communicate()
    graphObjOrig = graphInfo()
    graphObjOrig = readGraph("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/pythonExperiments/experiments/05_kmeans/531_rocket20_k"+str(clusterCutoff)+"_graphDump")
    origSigToID, origIDToSig = getIDToSigFromHarness(graphObjOrig,"/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/TestHarness.h")
    activationList = getActivationList("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/activationDump.txt", 2000)
    #partitionList = getPartActList("/soe/alexlee/alexlee/essent-testbed/rocket20/partitionDump.txt", 2000)
    noInSrcList, noOutSrcList = getNoInNoOut(graphObjOrig)
    IDToSrcSrcList = find_sources_sig(graphObjOrig.outNeigh, noInSrcList)
    #eval
    import sys
    toWriteEvalTo = open("531_rocket20_k"+str(clusterCutoff)+"_eval.log", 'w')
    sysStdOutOrig = sys.stdout
    with toWriteEvalTo as sys.stdout:
        evaluateClusters(graphObjOrig, graphObjOrig.IDToMergeID, graphObjOrig.MergeIDToMembers, activationList, origSigToID, origIDToSig)
    sys.stdout = sysStdOutOrig
    
