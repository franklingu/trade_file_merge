#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <math.h>

#define INITIAL_SORT_ITEM_COUNT 1000000
#define NUM_WAYS_MERGE 128

#define TIME_SIZE 9
#define SYMBOL_SIZE 16
#define QUANTITY_SIZE 9
#define PRICE_SIZE 11
#define BID_SIZE_SIZE 7
#define ASK_SIZE_SIZE 7

struct Trade
{
    char prc[PRICE_SIZE];
    char qty[QUANTITY_SIZE];
    char symbol[SYMBOL_SIZE];
    char tm[TIME_SIZE];
};

struct NBBO
{
    char ask[PRICE_SIZE]; // ask price
    char asize[ASK_SIZE_SIZE]; // ask size
    char bid[PRICE_SIZE]; // bid price
    char bsize[BID_SIZE_SIZE]; // bid size
    char symbol[SYMBOL_SIZE];
    char tm[TIME_SIZE];
};

struct LineIndexEntry
{
    std::string line;
    int index;
};

void fillTrade(Trade & trade, std::string line)
{
    int start = 0;
    int end = start + TIME_SIZE;
    for (int i = start; i < end; ++i)
    {
        trade.tm[(i - start)] = line[i];
    }
    start = 10;
    end = start + SYMBOL_SIZE;
    for (int i = start; i < end; ++i)
    {
        trade.symbol[(i - start)] = line[i];
    }
    start = 30;
    end = start + QUANTITY_SIZE;
    for (int i = start; i < end; ++i)
    {
        trade.qty[(i - start)] = line[i];
    }
    start = 39;
    end = start + PRICE_SIZE;
    for (int i = start; i < end; ++i)
    {
        trade.prc[(i - start)] = line[i];
    }
}

void fillNBBO(NBBO & nbbo, std::string line)
{
    int start = 0;
    int end = start + TIME_SIZE;
    for (int i = start; i < end; ++i)
    {
        nbbo.tm[(i - start)] = line[i];
    }
    start = 10;
    end = start + SYMBOL_SIZE;
    for (int i = start; i < end; ++i)
    {
        nbbo.symbol[(i - start)] = line[i];
    }
    start = 26;
    end = start + PRICE_SIZE;
    for (int i = start; i < end; ++i)
    {
        nbbo.bid[(i - start)] = line[i];
    }
    start = 37;
    end = start + BID_SIZE_SIZE;
    for (int i = start; i < end; ++i)
    {
        nbbo.bsize[(i - start)] = line[i];
    }
    start = 44;
    end = start + PRICE_SIZE;
    for (int i = start; i < end; ++i)
    {
        nbbo.ask[(i - start)] = line[i];
    }
    start = 55;
    end = start + ASK_SIZE_SIZE;
    for (int i = start; i < end; ++i)
    {
        nbbo.asize[(i - start)] = line[i];
    }
}

template <typename A, typename B>
bool compareByTime(const A & t1, const B & t2)
{
	for (int i = 0; i < TIME_SIZE; ++i)
	{
		if (t1.tm[i] != t2.tm[i])
		{
			return (t1.tm[i] < t2.tm[i]);
		}
	}

	return false;
}

bool compareByTimeStr(const std::string & t1, const std::string & t2)
{
	for (int i = 0; i < TIME_SIZE; ++i)
	{
		if (t1[i] != t2[i])
		{
			return (t1[i] < t2[i]);
		}
	}

	return false;
}

void sortNBBOs(std::vector<NBBO> & nbbos)
{
    std::sort(nbbos.begin(), nbbos.end(), compareByTime<NBBO, NBBO>);
}

void fillTradeLine(char * tradeLine, const Trade & trade)
{
    int start = 0;
    for (int i = 0; i < TIME_SIZE; ++i)
    {
        tradeLine[start] = trade.tm[i];
        start++;
    }
    for (int i = 0; i < SYMBOL_SIZE; ++i)
    {
        tradeLine[start] = trade.symbol[i];
        start++;
    }
    for (int i = 0; i < QUANTITY_SIZE; ++i)
    {
        tradeLine[start] = trade.qty[i];
        start++;
    }
    for (int i = 0; i < PRICE_SIZE; ++i)
    {
        tradeLine[start] = trade.prc[i];
        start++;
    }
}

void fillNBBOLine(char * nbboLine, const NBBO & nbbo)
{
    int start = 0;
    for (int i = 0; i < TIME_SIZE; ++i)
    {
        nbboLine[start] = nbbo.tm[i];
        start++;
    }
    for (int i = 0; i < SYMBOL_SIZE; ++i)
    {
        nbboLine[start] = nbbo.symbol[i];
        start++;
    }
    for (int i = 0; i < PRICE_SIZE; ++i)
    {
        nbboLine[start] = nbbo.bid[i];
        start++;
    }
    for (int i = 0; i < BID_SIZE_SIZE; ++i)
    {
        nbboLine[start] = nbbo.bsize[i];
        start++;
    }
    for (int i = 0; i < PRICE_SIZE; ++i)
    {
        nbboLine[start] = nbbo.ask[i];
        start++;
    }
    for (int i = 0; i < ASK_SIZE_SIZE; ++i)
    {
        nbboLine[start] = nbbo.asize[i];
        start++;
    }
}

void writeToTmpOutput(const std::string filename, const std::vector<Trade> & trades)
{
    std::ofstream outfile(filename.c_str());
    std::cout << filename;
    char tradeLine[47];
    tradeLine[45] = '\n';
    tradeLine[46] = '\0';
    if (outfile.is_open())
    {
        for (int i = 0; i < trades.size(); ++i)
        {
            fillTradeLine(tradeLine, trades[i]);
            outfile << tradeLine;
        }
        outfile.close();
    }
    else
    {
        std::cout << "Unable to open file to write\n";
    }
}

std::string mergeSortedFiles(int fileCount, int numRun)
{
    char filenameBuf[128];  // to allow a huge fileCount and numRun
    int numRun = 0;
    std::vector<std::ifstream> inputFiles;
    LineIndexEntry inputEntries[NUM_WAYS_MERGE];
    std::string line;
    while (fileCount > 1)
    {
        int num = 0, mergeFileCount = 0;
        for (int i = 0; i < fileCount; ++i)
        {
            sprintf(filenameBuf, "tmp-trade-%d-%d.txt", numRun, i);
            std::ifstream currFile(filenameBuf);
            inputFiles.push_back(currFile);
            num++;
            if (num >= NUM_WAYS_MERGE)
            {
                // merge all files
                // so the basic idea here is to have vector of strings
                // and also vector of indexes(sorted--better min heap). write that line to out and try to get
                // a new line from that file. if it fails, then that file is done. remove that index
                // when only one file exists, this still works
                for (int j = 0; j < NUM_WAYS_MERGE; ++j)
                {
                    getline(inputFiles[i], line);
                    inputEntries[i].line = line;
                    inputEntries[i].index = i;
                }
                sprintf(filenameBuf, "tmp-trade-%d-%d.txt", numRun + 1, mergeFileCount);
                std::ofstream outfile(filenameBuf);
                // sort it
                // for smallest in inputEntries, output line to outfile
                // and refill it and sort it. if inputEntries empty, done
                for (int j = 0; j < inputFiles.size(); ++j)
                {
                    inputFiles[j].close();
                }
                inputFiles.erase(inputFiles.begin(), inputFiles.end());
                mergeFileCount++;
                num = 0;
            }
        }
        if (num > 0)
        {
            //merge the remaining
        }
        numRun++;
        fileCount = (int)ceil(fileCount * 1.0 / NUM_WAYS_MERGE;
    }
    sprintf(filenameBuf, "tmp-trade-%d-%d.txt", numRun, fileCount);
    return std::string(filenameBuf);
}

void processTradeFile()
{
    std::vector<Trade> trades(INITIAL_SORT_ITEM_COUNT);
    std::string line;
    std::string tradeFileName("sample_trades.txt");
    std::ifstream infile1(tradeFileName.c_str());
    char filenameBuf[128];  // to allow a huge fileCount and numRun
    int numRun = 0, numCount = 0, fileCount = 0, itemLimit = INITIAL_SORT_ITEM_COUNT - 1;
    if (infile1.is_open())
    {
        getline(infile1, line);  // skip first line: not a trade record
        while (getline(infile1, line))
        {
            fillTrade(trades[numCount], line);
            numCount++;
            if (numCount >= itemLimit)
            {
                std::sort(trades.begin(), trades.end(), compareByTime<Trade, Trade>);
                sprintf(filenameBuf, "tmp-trade-%d-%d.txt", numRun, fileCount);
                std::string filename(filenameBuf);
                writeToTmpOutput(filename, trades);
                numCount = 0;
                fileCount++;
            }
        }
        if (numCount > 0)
        {
            std::sort(trades.begin(), trades.end(), compareByTime<Trade, Trade>);
            sprintf(filenameBuf, "tmp-trade-%d-%d.txt", numRun, fileCount);
            std::string filename(filenameBuf);
            writeToTmpOutput(filename, trades);
        }
        infile1.close();
    }
    else
    {
        std::cout << "File" << tradeFileName << " cannot be opened!\n";
    }
}

int main(int argc, char const *argv[])
{
    processTradeFile();

//    std::string nbboFileName("sample_nbbos.txt");
//    std::ifstream infile2(nbboFileName.c_str());
//    if (infile2.is_open())
//    {
//        getline(infile2, line);  // skip first line: not a nbbo record
//        while (getline(infile2, line))
//        {
//            appendNBBO(nbbos, line);
//        }
//        infile2.close();
//    }
//    else
//    {
//        std::cout << "File" << nbboFileName << " cannot be opened!\n";
//    }

    return 0;
}
