#this script just reads in essent graph dump (affter mffc) mergeIDToMemebrs and compares it with the mffc output
import sys
import ast
sys.path.insert(0, '/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/pythonExperiments')
from partEvalFuncs.readGraph import *
#get mffc info
MountBaldy2022

#get validNode info
graphObjOrig = graphInfo()
graphObjOrig = readGraph("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/essent/051723_original_graphdump")
graphObj.validNodes = graphObjOrig.validNodes
#get mffc c produced
f = open("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/mffcdebug")
for l in f:
    continue
l = "["+l[5:-1]+"]"
mffcTest = ast.literal_eval(l)
#compare
probCount = 0
for mergeID in graphObj.MergeIDToMembers:
    for memb in graphObj.MergeIDToMembers[mergeID]:
        if mffcTest[memb] != mffcTest[graphObj.MergeIDToMembers[mergeID][0]] and memb in graphObj.validNodes:
            if not all([mffcTest[memb1] == mffcTest[graphObj.MergeIDToMembers[mergeID][0]] for memb1 in set(graphObj.MergeIDToMembers[mergeID])-set([memb])]):
                print("problem!")
                print(mergeID)
                print(graphObj.MergeIDToMembers[mergeID])
                print([mffcTest[i] for i in graphObj.MergeIDToMembers[mergeID]])
                print(memb)
            #exit(111)
            probCount += 1
        # else:
        #     print("ok!")
            # print(mergeID)
            # print(graphObj.MergeIDToMembers[mergeID])
            # print(memb)
print(len(graphObj.MergeIDToMembers), "vs", probCount)