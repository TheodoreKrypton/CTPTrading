#pragma comment(lib, ".\\thostmduserapi.lib")
#pragma comment(lib, ".\\thosttraderapi.lib")

#include "Query.h"
//#include "Trader.h"
#include "ThostFtdcMdApi.h"
#include "ThostFtdcTraderApi.h"

#ifdef _WIN32
#include<Windows.h>
#endif


int main()
{
	// ��ʼ��ѯ�۶���
	Query* query = new Query;
	query->Init();

	// ��Ӻ�Լ
	char* instruments[1] = { "au1706" };
	query->SubscribeMarketData(instruments, 1);

	// ѯ��
	while (true)
	{
		std::cout << query->MarketData.AskPrice1 << std::endl;
#ifdef _WIN32
		Sleep(500);
#endif
	}

	
	delete query;
}