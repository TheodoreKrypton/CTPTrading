#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "thostmduserapi.lib")
#pragma comment(lib, "thosttraderapi.lib")

#include "Common.h"
#include "Query.h"
#include "Trade.h"

#include "price.h"
#include "command.h"

#include<iomanip>
#include<vector>


int nMdRequestID = 0;
int nTdRequestID = 0;
CThostFtdcDepthMarketDataField* MarketData;
char* FRONTADD_MD = "tcp://180.168.146.187:10010";
char* FRONTADD_TD = "tcp://180.168.146.187:10000";
int SESSION;
int FRONTID;
TThostFtdcOrderRefType ORDER_REF;
const char* USERID = "079056";
const char* PASSWD = "123456";
const char* BROKER = "9999";
const char* XCHGER = "SHFE";
CThostFtdcMdApi* mdapi;
CThostFtdcTraderApi* tdapi;
MdSpi* mdspi;
TdSpi* tdspi;
bool mdlogin = false;
bool tdlogin = false;
char* ppInstrumentID[1] = { "au1706" };
int iInstrumentID = 1;
std::vector<TThostFtdcTradeIDType> trade_orders;
Logger lgr("log.txt");
extern bool EAEnabled = false;

/*
	�߳�0�����۸���
	�߳�1����������
*/
std::vector<std::thread> threads(5);

bool output_mutex = false;

int main()
{
	InitScreen();
	InitQuery();
	while (!mdlogin);
	InitTrade();
	while (!tdlogin);
	mdapi->SubscribeMarketData(ppInstrumentID, iInstrumentID);
	InitThreads();
	
	OrderSend(ppInstrumentID[0], THOST_FTDC_OF_CloseToday, THOST_FTDC_D_Sell, 1);
	
	return 0;
}
