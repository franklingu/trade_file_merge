#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <queue>
#include <math.h>

#define INITIAL_SORT_ITEM_COUNT 2000  // number of records written to one file in the initial prepare run
#define NUM_WAYS_MERGE 200            // number of way_of_merge. e.g.: if it is 200, then 200 files will be merged into one file
#define FILE_NAME_BUF_SIZE 128        // temporary file name buffer size

// trade and nbbo file specification's definition, shoud not be changed
#define TIME_SIZE 9
#define SYMBOL_SIZE 16
#define QUANTITY_SIZE 9
#define PRICE_SIZE 11
#define BID_SIZE_SIZE 7
#define ASK_SIZE_SIZE 7
#define TIME_START_POS 0
#define SYMBOL_START_POS 10
#define QUANTITY_TRADE_START_POS 30
#define PRICE_TRADE_START_POS 39
#define BID_PRICE_START_POS 26
#define BID_SIZE_START_POS 37
#define ASK_PRICE_START_POS 44
#define ASK_SIZE_START_POS 55

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

bool compareTimeString( const std::string & t1, const std::string & t2, bool greater=false)
{
    for (int i = 0; i < TIME_SIZE; ++i)
    {
        if (t1[i] != t2[i])
        {
            return greater ? (t1[i] > t2[i]) : (t1[i] < t2[i]);
        }
    }

    return false;
}

class LineEntryCompare
{
    bool reverse;
public:
    LineEntryCompare(const bool& revparam=false)
    {
        reverse=revparam;
    }
    bool operator() (const LineIndexEntry& lhs, const LineIndexEntry& rhs) const
    {
        // a greater comparator makes min heap out of pq
        bool result = compareTimeString(lhs.line, rhs.line, true);

        return reverse ? !result : result;
    }
};

typedef std::priority_queue<LineIndexEntry, std::vector<LineIndexEntry>, LineEntryCompare> LineEntryPQ;

void fillTradeLine(std::string & tradeLine, std::string line)
{
    int start = TIME_START_POS;
    int end = start + TIME_SIZE;
    int runner = 0;
    for (int i = start; i < end; ++i)
    {
        tradeLine[runner] = line[i];
        runner++;
    }
    start = SYMBOL_START_POS;
    end = start + SYMBOL_SIZE;
    for (int i = start; i < end; ++i)
    {
        tradeLine[runner] = line[i];
        runner++;
    }
    start = QUANTITY_TRADE_START_POS;
    end = start + QUANTITY_SIZE;
    for (int i = start; i < end; ++i)
    {
        tradeLine[runner] = line[i];
        runner++;
    }
    start = PRICE_TRADE_START_POS;
    end = start + PRICE_SIZE;
    for (int i = start; i < end; ++i)
    {
        tradeLine[runner] = line[i];
        runner++;
    }
}

void fillNBBOLine(std::string & nbboLine, std::string line)
{
    int start = TIME_START_POS;
    int end = start + TIME_SIZE;
    int runner = 0;
    for (int i = start; i < end; ++i)
    {
        nbboLine[runner] = line[i];
        runner++;
    }
    start = SYMBOL_START_POS;
    end = start + SYMBOL_SIZE;
    for (int i = start; i < end; ++i)
    {
        nbboLine[runner] = line[i];
        runner++;
    }
    start = BID_PRICE_START_POS;
    end = start + PRICE_SIZE;
    for (int i = start; i < end; ++i)
    {
        nbboLine[runner] = line[i];
        runner++;
    }
    start = BID_SIZE_START_POS;
    end = start + BID_SIZE_SIZE;
    for (int i = start; i < end; ++i)
    {
        nbboLine[runner] = line[i];
        runner++;
    }
    start = ASK_PRICE_START_POS;
    end = start + PRICE_SIZE;
    for (int i = start; i < end; ++i)
    {
        nbboLine[runner] = line[i];
        runner++;
    }
    start = ASK_SIZE_START_POS;
    end = start + ASK_SIZE_SIZE;
    for (int i = start; i < end; ++i)
    {
        nbboLine[runner] = line[i];
        runner++;
    }
}

bool compareByTimePart(const std::string & t1, const std::string & t2)
{
    return compareTimeString(t1, t2);
}

void writeToInitialTmpOutput(const std::string filename, const std::vector<std::string> & records)
{
    std::ofstream outfile(filename);
    std::cout << "Tmp file: " << filename << "\n";
    if (outfile.is_open())
    {
        int size = records.size();
        for (int i = 0; i < size; ++i)
        {
            outfile << records[i] << "\n";
        }
        outfile.close();
    }
    else
    {
        std::cout << "Unable to open file to write\n";
    }
}

void mergeFilesToNextRun(std::vector<std::string> inputFilenames, LineEntryPQ &inputEntries,
                         const std::string & filenameFormat, int numRun, int mergeFileCount)
{
    std::ifstream inputFiles[NUM_WAYS_MERGE];
    std::string line;
    char filenameBuf[FILE_NAME_BUF_SIZE];
    for (int j = 0; j < inputFilenames.size(); ++j)
    {
        inputFiles[j] = std::ifstream(inputFilenames[j]);
    }
    for (int j = 0; j < inputFilenames.size(); ++j)
    {
        if (!getline(inputFiles[j], line))
        {
            continue;
        }
        LineIndexEntry entry;
        entry.line = line;
        entry.index = j;
        inputEntries.push(entry);
    }
    sprintf_s(filenameBuf, FILE_NAME_BUF_SIZE, filenameFormat.c_str(), numRun + 1, mergeFileCount);
    std::ofstream outfile(filenameBuf);
    while (!inputEntries.empty())
    {
        LineIndexEntry entry = inputEntries.top();
        inputEntries.pop();
        if (getline(inputFiles[entry.index], line))
        {
            LineIndexEntry refill;
            refill.index = entry.index;
            refill.line = line;
            inputEntries.push(refill);
        }
        outfile << entry.line << "\n";
    }
    outfile.close();
    std::cout << "Tmp file: " << filenameBuf << "\n";
    for (int j = 0; j < inputFilenames.size(); ++j)
    {
        inputFiles[j].close();
    }
}

std::string mergeSortedFiles(const std::string & filenameFormat, int numRun, int fileCount)
{
    char filenameBuf[FILE_NAME_BUF_SIZE];
    std::vector<std::string> inputFiles;
    LineEntryPQ inputEntries;
    std::string line;
    while (fileCount > 1)
    {
        int num = 0, mergeFileCount = 0;
        for (int i = 0; i < fileCount; ++i)
        {
            sprintf_s(filenameBuf, FILE_NAME_BUF_SIZE, filenameFormat.c_str(), numRun, i);
            inputFiles.push_back(std::string(filenameBuf));
            num++;
            if (num >= NUM_WAYS_MERGE)
            {
                mergeFilesToNextRun(inputFiles, inputEntries, filenameFormat, numRun, mergeFileCount);
                inputFiles.erase(inputFiles.begin(), inputFiles.end());
                mergeFileCount++;
                num = 0;
            }
        }
        if (num > 0)
        {
            mergeFilesToNextRun(inputFiles, inputEntries, filenameFormat, numRun, mergeFileCount);
            inputFiles.erase(inputFiles.begin(), inputFiles.end());
            mergeFileCount++;
        }
        numRun++;
        fileCount = (int)ceil(fileCount * 1.0 / NUM_WAYS_MERGE);
    }
    sprintf_s(filenameBuf, FILE_NAME_BUF_SIZE, filenameFormat.c_str(), numRun, fileCount - 1);
    return std::string(filenameBuf);
}

std::string processRawFile(bool forTrade)
{
    std::vector<std::string> records(INITIAL_SORT_ITEM_COUNT);
    std::string line;
    std::string fileName;
    std::string fileNameFormat;
    char filenameBuf[FILE_NAME_BUF_SIZE];
    int numRun = 0, numCount = 0, fileCount = 0;
    int lineSize = TIME_SIZE + PRICE_SIZE + SYMBOL_SIZE + QUANTITY_SIZE;
    if (forTrade)
    {
        fileName = "sample_trades.txt";
        fileNameFormat = ".\\tmp\\tmp-trade-%d-%d.txt";
    }
    else
    {
        fileName = "sample_nbbos.txt";
        fileNameFormat = ".\\tmp\\tmp-nbbo-%d-%d.txt";
        lineSize = TIME_SIZE + SYMBOL_SIZE + PRICE_SIZE + QUANTITY_SIZE + PRICE_SIZE + QUANTITY_SIZE;
    }
    std::ifstream infile1(fileName);
    for (int i = 0; i < records.size(); ++i)
    {
        records[i].resize(lineSize, ' ');
    }
    if (infile1.is_open())
    {
        getline(infile1, line);  // skip first line: not a record
        while (getline(infile1, line))
        {
            if (forTrade)
            {
                fillTradeLine(records[numCount], line);
            }
            else
            {
                fillNBBOLine(records[numCount], line);
            }
            numCount++;
            if (numCount >= INITIAL_SORT_ITEM_COUNT)
            {
                std::sort(records.begin(), records.end(), compareByTimePart);
                sprintf_s(filenameBuf, FILE_NAME_BUF_SIZE, fileNameFormat.c_str(), numRun, fileCount);
                std::string filename(filenameBuf);
                writeToInitialTmpOutput(filename, records);
                numCount = 0;
                fileCount++;
            }
        }
        if (numCount > 0)
        {
            records.erase(records.begin() + numCount, records.end());
            std::sort(records.begin(), records.end(), compareByTimePart);
            sprintf_s(filenameBuf, FILE_NAME_BUF_SIZE, fileNameFormat.c_str(), numRun, fileCount);
            std::string filename(filenameBuf);
            writeToInitialTmpOutput(filename, records);
            fileCount++;
        }
        infile1.close();
        return mergeSortedFiles(fileNameFormat, numRun, fileCount);
    }
    else
    {
        std::cout << "File" << fileName << " cannot be opened!\n";
        return "";
    }
}

void mergeTradeAndNBBO(std::string tradeFileName, std::string nbboFileName)
{
    std::ifstream infileTrade(tradeFileName);
    std::ifstream infileNBBO(nbboFileName);
    std::string tradeLine, nbboLine;
    std::ofstream outfile("output.txt");
    if (infileTrade.is_open() && infileNBBO.is_open())
    {
        getline(infileTrade, tradeLine);
        getline(infileNBBO, nbboLine);
        while (true)
        {
            if (compareByTimePart(tradeLine, nbboLine))
            {
                outfile << tradeLine << "\n";
                if (!getline(infileTrade, tradeLine))
                {
                    break;
                }
            } 
            else
            {
                outfile << nbboLine << "\n";
                if (!getline(infileNBBO, nbboLine))
                {
                    break;
                }
            }
        }
        while (getline(infileTrade, tradeLine))
        {
            outfile << tradeLine << "\n";
        }
        while (getline(infileNBBO, nbboLine))
        {
            outfile << nbboLine << "\n";
        }
    }
    else
    {
        std::cout << tradeFileName << " or " << nbboFileName << " cannot be open!\n";
    }
}

int main(int argc, char const *argv[])
{
    system("rm -rf tmp");
    system("mkdir tmp");
    std::string tradeMergeFileName = processRawFile(true);
    std::string nbboMergeFileName = processRawFile(false);
    std::cout << "Final trade merged file: " << tradeMergeFileName << "\n";
    std::cout << "Final nbbo merged file: " << nbboMergeFileName << "\n";
    mergeTradeAndNBBO(tradeMergeFileName, nbboMergeFileName);
    system("rm -rf tmp");

    return 0;
}
