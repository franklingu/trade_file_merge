nyxdata Trade and NBBO file merging
===========================================

### Introduction

A simple C++ program to merge trade file and NBBO file from nyxdata into a time-series file.

### Setup

Compile worker.cpp and provide your trade data file with name "sample_trades.txt" and nbbo data file with name "sample_nbbos.txt".

### Customization

```cpp
#define INITIAL_SORT_ITEM_COUNT 2000
#define NUM_WAYS_MERGE 200
#define FILE_NAME_BUF_SIZE 128
```

You can adjust `INITIAL_SORT_ITEM_COUNT` and `NUM_WAYS_MERGE` in the code block above for performance tuning. `FILE_NAME_BUF_SIZE` is set to 128 and usually this should be more than enough to hold potential temporary file names. You can make it larger or smaller based on your needs.
