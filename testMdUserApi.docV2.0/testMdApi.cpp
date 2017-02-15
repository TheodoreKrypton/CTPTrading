// testTraderApi.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include ".\ThostTraderApi\ThostFtdcMdApi.h"
#include "MdSpi.h"

#include<iostream>
#include<Windows.h>
using namespace std;

// UserApi����
CThostFtdcMdApi* pUserApi;

// ���ò���
char FRONT_ADDR[] = "tcp://180.168.146.187:10010";		// ǰ�õ�ַ
TThostFtdcBrokerIDType	BROKER_ID = "9999";				// ���͹�˾����
TThostFtdcInvestorIDType INVESTOR_ID = "082477";			// Ͷ���ߴ���
TThostFtdcPasswordType  PASSWORD = "qidl2015";			// �û�����
char *ppInstrumentID[] = {"au1706"};			// ���鶩���б�
int iInstrumentID = 1;// ���鶩������
extern double lastprice = 0;

// ������
int iRequestID = 0;

void main(void)
{
	// ��ʼ��UserApi
	pUserApi = CThostFtdcMdApi::CreateFtdcMdApi();			// ����UserApi
	CMdSpi* pUserSpi = new CMdSpi();
	pUserApi->RegisterSpi(pUserSpi);						// ע���¼���
	pUserApi->RegisterFront(FRONT_ADDR);					// connect
	pUserApi->Init();

	pUserApi->SubscribeMarketData(ppInstrumentID, iInstrumentID);
	while (!pUserSpi->_login);
	while (1)
	{
		cout << ppInstrumentID[0] << ": " << lastprice << endl;
		Sleep(1000);
	}

	pUserApi->Join();
//	pUserApi->Release();
}