All the same stuff as essent; some notes for running and comparing this

External partitioner
--------------------------------------------------------------------------------
-go into externalPart_2
-make a.out with g++ acyclicPart.cpp Graph.cpp Index.cpp IntervalList.cpp Timer.cpp -O5 -ggdb -fopenmp
-make essent
-run with the following flags to use external partitioner:
./utils/bin/essent --part-cutoff 20 --activity-signal --graphDump --externalPartitioner <file to dump graph info in> <fir file>

Example invocations:

    $ ./essent --help
    $ ./essent -O1 my_design.fir

Interfacing with the generated code
--------------------------------------------------------------------------------

-running with the --externalPartition option will make the graphDump to data directory have neighbors
