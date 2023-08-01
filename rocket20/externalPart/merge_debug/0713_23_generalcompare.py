def getListOfID(line):
    split1 = line.split("<1>")[:-1]
    return [[int(i) for i in l.split("<2>")[:-1]] for l in split1]

f1 = open("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/072423_mergesToConsider_cpp")
f2 = open("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/externalPart/merge_debug/logfile_dump/072423_mergesToConsider_scala")
flines_cpp = [l for l in f1]
flines_scala = [l for l in f2]
#1: process the string
keyword = "mergesToConsider"
IdxList_cpp = list(filter(lambda idx : flines_cpp[idx].strip().lower() == keyword.lower(), range(len(flines_cpp))))
IdxList_scala = list(filter(lambda idx : flines_scala[idx].strip().lower() == keyword.lower(), range(len(flines_scala))))
checkIdx = 1
listOfIDs_cpp = getListOfID(flines_cpp[IdxList_cpp[checkIdx]+1])
listOfIDs_scala = getListOfID(flines_scala[IdxList_scala[checkIdx]+1])
#2 canonicalize
listOfIDs_cpp = [tuple(sorted(l)) for l in listOfIDs_cpp]
listOfIDs_scala = [tuple(sorted(l)) for l in listOfIDs_scala]
#3 compare

setOfListOfIDs_cpp = set(listOfIDs_cpp)
print([l in setOfListOfIDs_cpp for l in listOfIDs_scala].count(False))


