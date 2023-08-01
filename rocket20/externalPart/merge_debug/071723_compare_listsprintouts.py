#
# This just reads the mffc's from cpp and scala and compares them
# 

f1 = open("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/essent/071723_logmssg")
f2 = open("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/essent/071723_cpplogmsg")
mffc1 = [l for l in f1]
mffc2 = [l for l in f2]
#1: the mffcs are not the same after the first iteration
newMFFCseeds1 = [int(i) for i in mffc1[82][:-2].split(" ")]
newMFFCseeds2 = [int(i) for i in mffc2[6][:-2].split(", ")[:-1]]
print(len(newMFFCseeds1),len(newMFFCseeds2))
for i in range(len(newMFFCseeds1)):
    if newMFFCseeds1[i] != -2:
        if newMFFCseeds1[i] != newMFFCseeds2[i]:
            print("problem:", i, newMFFCseeds1[i])


