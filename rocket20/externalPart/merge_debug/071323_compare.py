f1 = open("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/essent/mffcdebugdump.txt")
f2 = open("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/essent/mffcdebugdumpFromCpp")
mffc1 = [l for l in f1]
mffc2 = [l for l in f2]
#1: the mffcs are not the same after the first iteration
# newMFFCseeds1 = set([int(i) for i in mffc1[10][:-2].split(" ")])
# newMFFCseeds2 = set([int(i) for i in mffc2[5][:-2].split(" ")])
# newMFFCseeds1 = set([int(i) for i in mffc1[23][:-2].split(" ")])
# newMFFCseeds2 = set([int(i) for i in mffc2[6][:-2].split(" ")])
# newMFFCseeds1 = set([int(i) for i in mffc1[36][:-2].split(" ")])
# newMFFCseeds2 = set([int(i) for i in mffc2[7][:-2].split(" ")])
# newMFFCseeds1 = set([int(i) for i in mffc1[49][:-2].split(" ")])
# newMFFCseeds2 = set([int(i) for i in mffc2[8][:-2].split(" ")])
newMFFCseeds1 = set([int(i) for i in mffc1[62][:-2].split(" ")])
newMFFCseeds2 = set([int(i) for i in mffc2[9][:-2].split(" ")])
print(len(newMFFCseeds1), len(newMFFCseeds2))
#2: the univistedSinnks are correct, the unvistedFringe is not
#@2B: it looks like the unvifisted fringe is correct now, but the unvisited fringe isn't?
unvisitedSinks1 = set([int(i) for i in mffc1[2][:-2].split(" ")])
all([(i in newMFFCseeds2) for i in unvisitedSinks1])
unvisitedFringe1 = set([int(i) for i in mffc1[8][:-2].split(" ")])
all([(i in newMFFCseeds2) for i in unvisitedFringe1])
#are the excluded the same? it seems so (first visited is only excluded nodes)
import sys
import ast
sys.path.insert(0, '/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/pythonExperiments')
from partEvalFuncs.readGraph import *
graphObj = graphInfo()
graphObj = readGraph("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/essent/070723_rocket20_k20_graphDump")
invalidNodes = set(range(len(graphObj.inNeigh))) - set(graphObj.validNodes)
visited1 = set([int(i) for i in mffc1[4][:-2].split(" ")])
len(visited1-invalidNodes)
#are the fringes the same?
fringe1 = set([int(i) for i in mffc1[6][:-2].split(" ")])
[(i in newMFFCseeds2) for i in fringe1]


