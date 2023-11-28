Additional things added here

--------------------------------------------------------------------------------

-outputGNeighbors_modfile and outputMGMergeInfo_modfile outputted for both interal and external partitioning; this outputs the graph info AFTER partitioning

-(eg to use to evaluate the internal partitioning)


External partitioner

--------------------------------------------------------------------------------

This is just a copy of essent that adds a flag so you can call the external partitioner. 

-go into externalPart_2

-make a.out with g++ acyclicPart.cpp Graph.cpp Index.cpp IntervalList.cpp Timer.cpp -O5 -ggdb -fopenmp

-make essent

-run with the following flags to use external partitioner:

./utils/bin/essent --part-cutoff 20 --activity-signal --graphDump --externalPartitioner <file to dump graph info in> <fir file>

-some relevant info about where info is dumped

https://docs.google.com/document/d/1ah0p9xHX_dHAqMR7jxJcyLR8zryhvBTA06zx_bLZ_s4/edit?usp=sharing
