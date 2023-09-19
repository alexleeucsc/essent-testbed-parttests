external profiling checkpoint 2:

main ideas for acceleration:

1: mergeSmallPartsLayered, which uses longest-path indexing to find neighbors that can be merged (1) without changing the indexing and (2) without checking merge path

2: use reached.insert(nextFrontier.begin(), nextFrontier.end()); + pass-as-reference

3: use only one set in orderConstrSibs