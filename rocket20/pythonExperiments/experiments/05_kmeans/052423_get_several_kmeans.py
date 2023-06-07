import subprocess

for clusterCount in range(400,2000,50):
    file_ = open("kmeansTest_"+str(clusterCount)+".log", "w")
    subprocess.Popen(["python3", "/soe/alexlee/alexlee/essent-testbed-document/essent-testbed-parttests/rocket20/pythonExperiments/experiments/05_kmeans/050923_reduceAndKmeans_clusters.py", str(clusterCount)], stdout=file_)