import sys
import ast
sys.path.insert(0, '/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/pythonExperiments')
flines = [l for l in open("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart_2_topotest/debuggraphinfo")]
def getIDToListOfID(l):
    outVec = [[] for _ in range(len(l.split("<1>")))]
    outAsStr = [l2.split("<2>")[:-1] for l2 in l.split("<1>")]
    for i in range(len(outVec)):
        outVec[i] = [int(mem) for mem in outAsStr[i]]
    return outVec
inNeigh = getIDToListOfID(flines[0][8:])
outNeigh = getIDToListOfID(flines[1][9:])
from collections import deque

from collections import deque, defaultdict

def longest_path(outNeigh, inNeigh):
    # Calculate in-degrees of nodes
    inDegree = defaultdict(int)
    for i, neighbors in enumerate(inNeigh):
        inDegree[i] = len(neighbors)
    
    # Initialize list to store topological sort
    topoSort = []
    # Queue to store nodes with in-degree 0
    zeroInDegree = deque(i for i, degree in inDegree.items() if degree == 0)
    
    # If a node is not in inDegree, it means it has in-degree of 0
    for i in range(len(outNeigh)):
        if i not in inDegree:
            zeroInDegree.append(i)
    print("zeroInDegree:",len(zeroInDegree))
    while zeroInDegree:
        node = zeroInDegree.popleft()
        topoSort.append(node)
        
        for neigh in outNeigh[node]:
            inDegree[neigh] -= 1
            if inDegree[neigh] == 0:
                zeroInDegree.append(neigh)
    
    # if len(topoSort) < len(outNeigh):
    #     print("The graph has a cycle.")
    #     return
    
    longestPaths = {node: (0, [node]) for node in range(len(outNeigh))}  # Initialize paths
    
    for node in topoSort:
        current_length, current_path = longestPaths[node]
        
        for neighbor in outNeigh[node]:
            if neighbor not in longestPaths:  # Neighbor not visited yet, graph has a cycle
                print("The graph has a cycle.")
                return
            
            new_length = current_length + 1
            new_path = current_path + [neighbor]
            
            if new_length > longestPaths[neighbor][0]:
                longestPaths[neighbor] = (new_length, new_path)
    
    return longestPaths


dists = longest_path(outNeigh, inNeigh)

a = 11365
b = 13455
outNeigh[a].append(outNeigh[b])
inNeigh[a].append(inNeigh[b])
outNeigh[b] = []
inNeigh[b] = []

dists2 = longest_path(outNeigh, inNeigh)
