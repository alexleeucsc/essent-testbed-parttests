#
# This just reads the mffc's from cpp and scala and compares them
# 

test_eq = False

f1 = open("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/essent/080623_scalalogmssg")
f2 = open("/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/essent/080623_cpplogmsg")
mffc1 = [l for l in f1]
mffc2 = [l for l in f2]
#1: the mffcs are not the same after the first iteration
newMFFCseeds1 = [int(i) for i in mffc1[176][:-1].split(", ")[:-1]]
newMFFCseeds2 = [int(i) for i in mffc2[85][:-1].split(", ")[:-1]]
print(len(newMFFCseeds1),len(newMFFCseeds2))
if test_eq:
    for i in range(len(newMFFCseeds1)):
        if newMFFCseeds1[i] != -2:
            if newMFFCseeds1[i] != newMFFCseeds2[i]:
                print("problem:", i, newMFFCseeds1[i])
else:
    for i in range(len(newMFFCseeds1)):
            if not newMFFCseeds1[i] in newMFFCseeds2:
                print("problem:", i, newMFFCseeds1[i])


