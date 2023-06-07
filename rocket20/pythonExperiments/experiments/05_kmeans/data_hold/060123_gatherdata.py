
# import required module
import os
directory = '/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/pythonExperiments/experiments/05_kmeans/data_hold'
clustLimToEval = {}
x,y = [],[]
for filename in os.listdir(directory):
    if "_eval.log" in filename:
        print(filename)
        clusterLimit = int(filename[filename.index("20_k")+4:filename.index("_eval.log")])
        f = os.path.join(directory, filename)
        if os.path.isfile(f):
            fileObj = open(f,"r")
            lastLine = ""
            for l in fileObj:
                if len(l)>2:
                    lastLine = l
            act,reqAct,diffAct = lastLine.split()
            act,reqAct,diffAct = float(act),float(reqAct),float(diffAct)
            print("act,reqAct,diffAct: ", act,reqAct,diffAct)
            y.append(diffAct)
    if "_graphDumpStatistics.log" in filename:
        print(filename)
        clusterLimit = int(filename[filename.index("20_k")+4:filename.index("_graphDumpStatistics.log")])
        f = os.path.join(directory, filename)
        if os.path.isfile(f):
            fileObj = open(f,"r")
            partsLine = ""
            for l in fileObj:
                if "Parts:" in l:
                    partsLine = l
            partCount = int(partsLine[partsLine.index(" ")+1:])
            print("partCount:",partCount)
            x.append(partCount)
print("---------x and y----------")
print(x)
print(y)

[1997, 1730, 1560, 1444, 1224, 1116, 1055, 972, 921, 872, 835, 774, 737, 682, 642, 631, 610, 593, 565, 551, 540, 530, 512, 480, 462]
[0.11705131218174696, 0.1216939482961222, 0.11103532119075596, 0.10659994124559341, 0.12682791813552682, 0.13268390129259697, 0.14024500587544064, 0.13479921660791228, 0.15280871523697612, 0.15611075205640423, 0.1512219741480611, 0.1753839992166079, 0.1727219545632589, 0.17487155307481395, 0.1841671758715237, 0.17795332941637287, 0.1839042988640815, 0.18406722483352916, 0.1886193008225617, 0.21219907951429692, 0.1959855072463768, 0.21003836662749706, 0.1997608597728163, 0.20891612808460636, 0.21247841754798277]        