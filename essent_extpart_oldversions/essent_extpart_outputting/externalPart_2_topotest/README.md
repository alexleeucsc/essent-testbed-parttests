v2 diff:

added the file io to enable evaluating for activation ratio


external profiling checkpoint 2:

main ideas for acceleration:

1: mergeSmallPartsLayered, which uses longest-path indexing to find neighbors that can be merged (1) without changing the indexing and (2) without checking merge path

1b: use mergeLimited, which allows the mergeReq that lead to very short searches to be done first

1c: mergeSmallPartsLayeredDown; uses longest-path indexing, plus the observation that
    (1) you can merge nodes in layer i to nodes in layer i+1 if the layer-indexing is correct
    (2) merging nodes at layers (i,i+1) only changes the longest-path to nodes in layers greater than i
    (2b)... meaning if we merge nodes starting from the bottom layer, we will merge mergeReqs above it after this merge is done,
        the mergeReq is only corrupted below layer i, so this can be done check-free
    (3) finally, mergeReq in the same layer are only illegal if they share children

1c2: some extra hacking: mspld is slow because it finds relatively few merges to do every iteration. By thresholding it, we would get very few merges to do.
However, everytime we are done with a mspd, we get new oppertunities to do some cheap mspld merges.

2: use reached.insert(nextFrontier.begin(), nextFrontier.end()); + pass-as-reference

3: use only one set in orderConstrSibs

timings:


https://docs.google.com/document/d/19rwLhDkhisNFt-zNK04TRysohKsGIoZjc39rBC-aFjA/edit?usp=sharing

https://docs.google.com/document/d/1tjb933DlUBLGWj4Icngi6nuoN74a8mOoMleMy9CB6A8/edit?usp=sharing
