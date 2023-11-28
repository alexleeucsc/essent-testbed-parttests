external profiling checkpoint 2:

main ideas for acceleration:

1: mergeSmallPartsLayered, which uses longest-path indexing to find neighbors that can be merged (1) without changing the indexing and (2) without checking merge path

1b: use mergeLimited, which allows the mergeReq that lead to very short searches to be done first

2: use reached.insert(nextFrontier.begin(), nextFrontier.end()); + pass-as-reference

3: use only one set in orderConstrSibs

timings:


https://docs.google.com/document/d/19rwLhDkhisNFt-zNK04TRysohKsGIoZjc39rBC-aFjA/edit?usp=sharing
