#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

#define TIME_SIZE 9
#define SYMBOL_SIZE 16
#define QUANTITY_SIZE 9
#define PRICE_SIZE 11
#define BID_SIZE_SIZE 7
#define ASK_SIZE_SIZE 7


struct RawTrade
{
    char tm[TIME_SIZE];
    char venue;
    char symbol[SYMBOL_SIZE];
    char cond[4];
    char qty[QUANTITY_SIZE];
    char prc[PRICE_SIZE];
    char filler0;
    char corr[2];
    char filler1[18];
    char rchar;
    char nchar;
};

struct RawNBBO
{
    char tm[TIME_SIZE];
    char venue;
    char symbol[SYMBOL_SIZE];
    char filler0[65];
    char bid[PRICE_SIZE]; // bid price
    char bsize[BID_SIZE_SIZE]; // bid size
    char filler1[8];
    char ask[PRICE_SIZE]; // ask price
    char asize[ASK_SIZE_SIZE]; // ask size
    char filler2[7];
    char filler3[2]; // since 2013/02/05
    char rchar;
    char nchar;
};

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

void appendTrade(std::vector<Trade> & trades, std::string line)
{
    Trade trade;
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

    trades.push_back(trade);
}

void appendNBBO(std::vector<NBBO> & nbbos, std::string line)
{
    NBBO nbbo;
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

    nbbos.push_back(nbbo);
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

void sortTrades(std::vector<Trade> & trades)
{
    std::sort(trades.begin(), trades.end(), compareByTime<Trade, Trade>);
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

void writeToOutput(std::vector<Trade> & trades, std::vector<NBBO> & nbbos)
{
    std::ofstream outfile("output.txt");
    char tradeLine[47];
    char nbboLine[63];
    tradeLine[45] = '\n';
    tradeLine[46] = '\0';
    nbboLine[61] = '\n';
    nbboLine[62] = '\0';
    if (outfile.is_open())
    {
        int start1 = 0, start2 = 0;
        while (start1 < trades.size() && start2 < nbbos.size())
        {
            if (compareByTime<Trade, NBBO>(trades[start1], nbbos[start2]))
            {
                fillTradeLine(tradeLine, trades[start1]);
                outfile << tradeLine;
                start1++;
            }
            else
            {
                fillNBBOLine(nbboLine, nbbos[start2]);
                outfile << nbboLine;
                start2++;
            }
        }
        while (start1 < trades.size())
        {
            fillTradeLine(tradeLine, trades[start1]);
            outfile << tradeLine;
            start1++;
        }
        while (start2 < nbbos.size())
        {
            fillNBBOLine(nbboLine, nbbos[start2]);
            outfile << nbboLine;
            start2++;
        }
        outfile.close();
    }
    else
    {
        std::cout << "Unable to open file to write\n";
    }
}

int main(int argc, char const *argv[])
{
    std::vector<Trade> trades;
    std::vector<NBBO> nbbos;

    std::string line;
    std::string tradeFileName("sample_trades.txt");
    std::ifstream infile1(tradeFileName.c_str());
    if (infile1.is_open())
    {
        getline(infile1, line);  // skip first line: not a trade record
        while (getline(infile1, line))
        {
            appendTrade(trades, line);
        }
        infile1.close();
    }
    else
    {
        std::cout << "File" << tradeFileName << " cannot be opened!\n";
    }

    std::string nbboFileName("sample_nbbos.txt");
    std::ifstream infile2(nbboFileName.c_str());
    if (infile2.is_open())
    {
        getline(infile2, line);  // skip first line: not a nbbo record
        while (getline(infile2, line))
        {
            appendNBBO(nbbos, line);
        }
        infile2.close();
    }
    else
    {
        std::cout << "File" << nbboFileName << " cannot be opened!\n";
    }

    sortTrades(trades);
    sortNBBOs(nbbos);

    writeToOutput(trades, nbbos);

    return 0;
}
