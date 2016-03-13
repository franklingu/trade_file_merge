#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <queue>
#include <math.h>

#define INITIAL_SORT_ITEM_COUNT 1000
#define NUM_WAYS_MERGE 200

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

bool compareTimeString( const std::string & t1, const std::string & t2 )
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
		bool result = compareTimeString(lhs.line, rhs.line);

		return reverse ? !result : result;
	}
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
	return compareTimeString(t1.tm, t2.tm);
}

bool compareByTimeStr(const std::string & t1, const std::string & t2)
{
	return compareTimeString(t1, t2);
}

typedef std::priority_queue<LineIndexEntry, std::vector<LineIndexEntry>, LineEntryCompare> LineEntryPQ;

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

void writeToInitialTmpOutput(const std::string filename, const std::vector<Trade> & trades)
{
    std::ofstream outfile(filename);
    std::cout << "Tmp file: " << filename << "\n";
    char tradeLine[47];
    tradeLine[45] = '\n';
    tradeLine[46] = '\0';
    if (outfile.is_open())
    {
		int size = trades.size();
        for (int i = 0; i < size; ++i)
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

void writeToInitialNBBOTmpOutput(const std::string filename, const std::vector<NBBO> & nbbos)
{
	std::ofstream outfile(filename);
	std::cout << "Tmp file: " << filename << "\n";
	char nbboLine[63];
	nbboLine[61] = '\n';
	nbboLine[62] = '\0';
	if (outfile.is_open())
	{
		int size = nbbos.size();
		for (int i = 0; i < size; ++i)
		{
			fillNBBOLine(nbboLine, nbbos[i]);
			outfile << nbboLine;
		}
		outfile.close();
	}
	else
	{
		std::cout << "Unable to open file to write\n";
	}
}

void mergeFilesToNextRun(std::vector<std::string> inputFilenames, LineEntryPQ &inputEntries,
						 const char * filenameFormat, int numRun, int mergeFileCount)
{
	std::ifstream inputFiles[NUM_WAYS_MERGE];
	std::string line;
	char filenameBuf[128];  // to allow a huge fileCount and numRun
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
	sprintf_s(filenameBuf, 128, filenameFormat, numRun + 1, mergeFileCount);
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

std::string mergeSortedFiles(const char * filenameFormat, int numRun, int fileCount)
{
    char filenameBuf[128];  // to allow a huge fileCount and numRun
    std::vector<std::string> inputFiles;
	LineEntryPQ inputEntries;
    std::string line;
    while (fileCount > 1)
    {
        int num = 0, mergeFileCount = 0;
        for (int i = 0; i < fileCount; ++i)
        {
            sprintf_s(filenameBuf, 128, filenameFormat, numRun, i);
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
    sprintf_s(filenameBuf, 128, filenameFormat, numRun, fileCount - 1);
    return std::string(filenameBuf);
}

std::string processTradeFile()
{
    std::vector<Trade> trades(INITIAL_SORT_ITEM_COUNT);
    std::string line;
    std::string tradeFileName("sample_trades.txt");
    std::ifstream infile1(tradeFileName);
	std::string filenameFormat(".\\tmp\\tmp-trade-%d-%d.txt");
    char filenameBuf[128];  // to allow a huge fileCount and numRun
    int numRun = 0, numCount = 0, fileCount = 0;
    if (infile1.is_open())
    {
        getline(infile1, line);  // skip first line: not a trade record
        while (getline(infile1, line))
        {
            fillTrade(trades[numCount], line);
            numCount++;
            if (numCount >= INITIAL_SORT_ITEM_COUNT)
            {
                std::sort(trades.begin(), trades.end(), compareByTime<Trade, Trade>);
                sprintf_s(filenameBuf, 128, filenameFormat.c_str(), numRun, fileCount);
                std::string filename(filenameBuf);
                writeToInitialTmpOutput(filename, trades);
                numCount = 0;
                fileCount++;
            }
        }
        if (numCount > 0)
        {
			trades.erase(trades.begin() + numCount, trades.end());
            std::sort(trades.begin(), trades.end(), compareByTime<Trade, Trade>);
            sprintf_s(filenameBuf, 128, filenameFormat.c_str(), numRun, fileCount);
            std::string filename(filenameBuf);
            writeToInitialTmpOutput(filename, trades);
			fileCount++;
        }
        infile1.close();
		return mergeSortedFiles(filenameFormat.c_str(), numRun, fileCount);
    }
    else
    {
        std::cout << "File" << tradeFileName << " cannot be opened!\n";
		return "";
    }
}

std::string processNBBOFile()
{
	std::vector<NBBO> nbbos(INITIAL_SORT_ITEM_COUNT);
	std::string line;
	std::string tradeFileName("sample_nbbos.txt");
	std::ifstream infile1(tradeFileName);
	std::string filenameFormat(".\\tmp\\tmp-nbbo-%d-%d.txt");
	char filenameBuf[128];  // to allow a huge fileCount and numRun
	int numRun = 0, numCount = 0, fileCount = 0;
	if (infile1.is_open())
	{
		getline(infile1, line);  // skip first line: not a trade record
		while (getline(infile1, line))
		{
			fillNBBO(nbbos[numCount], line);
			numCount++;
			if (numCount >= INITIAL_SORT_ITEM_COUNT)
			{
				std::sort(nbbos.begin(), nbbos.end(), compareByTime<NBBO, NBBO>);
				sprintf_s(filenameBuf, 128, filenameFormat.c_str(), numRun, fileCount);
				std::string filename(filenameBuf);
				writeToInitialNBBOTmpOutput(filename, nbbos);
				numCount = 0;
				fileCount++;
			}
		}
		if (numCount > 0)
		{
			nbbos.erase(nbbos.begin() + numCount, nbbos.end());
			std::sort(nbbos.begin(), nbbos.end(), compareByTime<NBBO, NBBO>);
			sprintf_s(filenameBuf, 128, filenameFormat.c_str(), numRun, fileCount);
			std::string filename(filenameBuf);
			writeToInitialNBBOTmpOutput(filename, nbbos);
			fileCount++;
		}
		infile1.close();
		return mergeSortedFiles(filenameFormat.c_str(), numRun, fileCount);
	}
	else
	{
		std::cout << "File" << tradeFileName << " cannot be opened!\n";
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
			if (compareByTimeStr(tradeLine, nbboLine))
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
    std::string tradeMergeFileName = processTradeFile();
	std::string nbboMergeFileName = processNBBOFile();
	std::cout << tradeMergeFileName << "\n";
	std::cout << nbboMergeFileName << "\n";
	mergeTradeAndNBBO(tradeMergeFileName, nbboMergeFileName);
	system("rm -rf tmp");

    return 0;
}
