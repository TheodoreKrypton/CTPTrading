#define _CRT_SECURE_NO_WARNINGS

#include"QuerySpi.h"

#include<memory.h>
#include<iostream>
#include<Windows.h>

void MdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	std::cerr << "--->>> " << "OnRspError" << std::endl;
	IsErrorRspInfo(pRspInfo);
}

void MdSpi::OnFrontDisconnected(int nReason)
{
	std::cerr << "--->>> " << "OnFrontDisconnected" << std::endl;
	std::cerr << "--->>> Reason = " << nReason << std::endl;
}

void MdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	std::cerr << "--->>> " << "OnHeartBeatWarning" << std::endl;
	std::cerr << "--->>> nTimerLapse = " << nTimeLapse << std::endl;
}

void MdSpi::OnFrontConnected()
{
	login = false;
	std::cerr << "--->>> " << "OnFrontConnected" << std::endl;
	///�û���¼����
	ReqUserLogin();
}

void MdSpi::ReqUserLogin()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER);
	strcpy(req.UserID, USERID);
	strcpy(req.Password, PASSWD);
	int iResult = mdapi->ReqUserLogin(&req, ++nRequestID);
	std::cerr << "--->>> �����û���¼����: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << std::endl;

}

void MdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cerr << "--->>> " << "OnRspUserLogin" << std::endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		login = true;
		///��ȡ��ǰ������
		std::cerr << "--->>> ��ȡ��ǰ������ = " << mdapi->GetTradingDay() << std::endl;
	}
}

void MdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cerr << "OnRspSubMarketData" << std::endl;
}

void MdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cerr << "OnRspUnSubMarketData" << std::endl;
}

void MdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	memcpy(MarketData, pDepthMarketData, sizeof(CThostFtdcDepthMarketDataField));
}

bool MdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// ���ErrorID != 0, ˵���յ��˴������Ӧ
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		std::cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
	return bResult;
}