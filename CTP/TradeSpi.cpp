#define _CRT_SECURE_NO_WARNINGS

#include "TradeSpi.h"
#include <iostream>
#include <iomanip>

// �����ж�
bool IsFlowControl(int iResult)
{
	return ((iResult == -2) || (iResult == -3));
}

void TdSpi::OnFrontConnected()
{
	lgr << Log::t << "[td]Front Connected..." << Log::endl;
	///�û���¼����
	ReqUserLogin();
}

void TdSpi::ReqUserLogin()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER);
	strcpy(req.UserID, USERID);
	strcpy(req.Password, PASSWD);
	int iResult = tdapi->ReqUserLogin(&req, ++nTdRequestID);
	lgr << Log::t << "[td]Send Login Request... " << ((iResult == 0) ? "Successed" : "Failed") << Log::endl;
}

void TdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		// ����Ự����
		FRONTID = pRspUserLogin->FrontID;
		SESSION = pRspUserLogin->SessionID;
		int iNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		iNextOrderRef++;
		sprintf(ORDER_REF, "%d", iNextOrderRef);
		///��ȡ��ǰ������
		lgr << Log::t << "[td]Date: " << tdapi->GetTradingDay() << Log::endl;
		///Ͷ���߽�����ȷ��
		SleepFor(1000);
		ReqSettlementInfoConfirm();
	}
}

void TdSpi::ReqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER);
	strcpy(req.InvestorID, USERID);
	int iResult = tdapi->ReqSettlementInfoConfirm(&req, ++nTdRequestID);
	lgr << Log::t << "[td]Request Settlement Info Confirm... " << ((iResult == 0) ? "Successed" : "Failed") << Log::endl;
}

void TdSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		tdlogin = true;
		///�����ѯ��Լ
		lgr << Log::t << "[td]Instruments:" << Log::endl;
		for (int i = 0;i < iInstrumentID;i++)
		{
			ReqQryInstrument(ppInstrumentID[i]);
		}
	}
}

void TdSpi::ReqQryInstrument(const char* instrument)
{
	CThostFtdcQryInstrumentField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.InstrumentID, instrument);
	while (true)
	{
		SleepFor(1000);
		int iResult = tdapi->ReqQryInstrument(&req, ++nTdRequestID);
		if (!IsFlowControl(iResult))
		{
			lgr << Log::t << "[td]Request Query Instrument... " << ((iResult == 0) ? "Successed" : "Failed") << Log::endl;
			break;
		}
		else
		{
			lgr << Log::t << "[td]Request Query Instrument... " << "Flow Control" << Log::endl;
		}
	} // while
}

void TdSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		lgr << Log::t << "[td]Instrument Id:\t" << pInstrument->InstrumentID << Log::endl;
		///�����ѯ��Լ
		SleepFor(1000);
		ReqQryTradingAccount();
	}
}

void TdSpi::ReqQryTradingAccount()
{
	CThostFtdcQryTradingAccountField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER);
	strcpy(req.InvestorID, USERID);
	while (true)
	{
		SleepFor(1000);
		int iResult = tdapi->ReqQryTradingAccount(&req, ++nTdRequestID);
		if (!IsFlowControl(iResult))
		{
			lgr << Log::t << "[td]Request Query Trading Account... " << ((iResult == 0) ? "Successed" : "Failed") << Log::endl;
			break;
		}
		else
		{
			lgr << Log::t << "[td]Request Query Trading Account... " << "Flow Control" << Log::endl;
		}
	} // while
}

void TdSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		lgr << Log::t << Log::_2point << "[td]Balance: " << pTradingAccount->Balance << Log::endl;
		lgr << Log::t << Log::_2point << "[td]Available: " << pTradingAccount->Available << Log::endl;
		lgr << Log::t << Log::_2point << "[td]Close Profit: " << pTradingAccount->CloseProfit << Log::endl;
		lgr << Log::t << Log::_2point << "[td]Position Profit: " << pTradingAccount->PositionProfit << Log::endl;
		///�����ѯͶ���ֲ߳�
		SleepFor(1000);
		ReqQryInvestorPosition();
	}
}

void TdSpi::ReqQryInvestorPosition()
{
	CThostFtdcQryInvestorPositionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER);
	strcpy(req.InvestorID, USERID);
	strcpy(req.InstrumentID, ppInstrumentID[0]);
	while (true)
	{
		SleepFor(1000);
		int iResult = tdapi->ReqQryInvestorPosition(&req, ++nTdRequestID);
		if (!IsFlowControl(iResult))
		{
			lgr << Log::t << "[td]Request Query Investor Position... " << ((iResult == 0) ? "Successed" : "Failed") << Log::endl;
			break;
		}
		else
		{
			lgr << Log::t << "[td]Request Query Investor Position... " << "Flow Control" << Log::endl;
		}
	} // while
}

void TdSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		if (pInvestorPosition)
		{
			lgr << Log::t << "[td]Open Volume:" << pInvestorPosition->OpenVolume << Log::endl;
		}
		///����¼������
		//ReqOrderInsert(ppInstrumentID[0], THOST_FTDC_D_Buy);
	}
}

//void TdSpi::ReqOrderInsert(const char* instrument_id, char direction, double limit_price, double stop_price)
//{
//	CThostFtdcInputOrderField req;
//	memset(&req, 0, sizeof(req));
//	///���͹�˾����
//	strcpy(req.BrokerID, BROKER);
//	///Ͷ���ߴ���
//	strcpy(req.InvestorID, USERID);
//	///��Լ����
//	strcpy(req.InstrumentID, ppInstrumentID[0]);
//	///��������
//	//strcpy(req.OrderRef, ORDER_REF);
//	///�û�����
//	//	TThostFtdcUserIDType	UserID;
//	///�����۸�����: �޼�
//	req.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
//	///��������: 
//	req.Direction = direction;
//	///��Ͽ�ƽ��־: ����
//	req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
//	///���Ͷ���ױ���־
//	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
//	///�۸�
//	req.LimitPrice = 0;
//	///����: 1
//	req.VolumeTotalOriginal = 1;
//	///��Ч������: ������Ч
//	req.TimeCondition = THOST_FTDC_TC_GFD;
//	///GTD����
//	//	TThostFtdcDateType	GTDDate;
//	///�ɽ�������: �κ�����
//	req.VolumeCondition = THOST_FTDC_VC_AV;
//	///��С�ɽ���: 1
//	req.MinVolume = 1;
//	///��������: ����
//	req.ContingentCondition = THOST_FTDC_CC_Immediately;
//	///ֹ���
//	req.StopPrice = stop_price;
//	///ǿƽԭ��: ��ǿƽ
//	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
//	///�Զ������־: ��
//	req.IsAutoSuspend = 0;
//	///ҵ��Ԫ
//	//	TThostFtdcBusinessUnitType	BusinessUnit;
//	///������
//	//	TThostFtdcRequestIDType	RequestID;
//	///�û�ǿ����־: ��
//	req.UserForceClose = 0;
//
//	int iResult = tdapi->ReqOrderInsert(&req, ++nTdRequestID);
//	lgr << Log::t << "[td]Requst Order Insert... " << ((iResult == 0) ? "Successed" : "Failed") << Log::endl;
//}

void TdSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	lgr << Log::t << "[td]Order: #" << pInputOrder->OrderRef << Log::endl;
	IsErrorRspInfo(pRspInfo);
}

void TdSpi::ReqOrderAction(CThostFtdcOrderField *pOrder)
{
	static bool ORDER_ACTION_SENT = false;		//�Ƿ����˱���
	if (ORDER_ACTION_SENT)
		return;

	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
	///���͹�˾����
	strcpy(req.BrokerID, pOrder->BrokerID);
	///Ͷ���ߴ���
	strcpy(req.InvestorID, pOrder->InvestorID);
	///������������
	//	TThostFtdcOrderActionRefType	OrderActionRef;
	///��������
	strcpy(req.OrderRef, pOrder->OrderRef);
	///������
	//	TThostFtdcRequestIDType	RequestID;
	///ǰ�ñ��
	req.FrontID = FRONTID;
	///�Ự���
	req.SessionID = SESSION;
	///����������
	//	TThostFtdcExchangeIDType	ExchangeID;
	///�������
	//	TThostFtdcOrderSysIDType	OrderSysID;
	///������־
	req.ActionFlag = THOST_FTDC_AF_Delete;
	///�۸�
	//	TThostFtdcPriceType	LimitPrice;
	///�����仯
	//	TThostFtdcVolumeType	VolumeChange;
	///�û�����
	//	TThostFtdcUserIDType	UserID;
	///��Լ����
	strcpy(req.InstrumentID, pOrder->InstrumentID);

	int iResult = tdapi->ReqOrderAction(&req, ++nTdRequestID);
	lgr << Log::t << "--->>> ������������: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << Log::endl;
	ORDER_ACTION_SENT = true;
}

void TdSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	lgr << Log::t << "--->>> " << "OnRspOrderAction" << Log::endl;
	IsErrorRspInfo(pRspInfo);
}

///����֪ͨ
void TdSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	lgr << Log::t << "[td]Direction: " << (pOrder->Direction == '0' ? "Buy" : "Sell")
		<< ", Price: " << pOrder->LimitPrice
		<< ", Status: " << pOrder->StatusMsg << Log::endl;
}

///�ɽ�֪ͨ
void TdSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	lgr << Log::t << "[td]" << pTrade->InstrumentID
		<< " At Price: " << pTrade->Price << ", " 
		<< pTrade->Volume << " Traded" << Log::endl;
}

void TdSpi::OnRspQryTrade(
	CThostFtdcTradeField *pTrade,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	if (bIsLast)
	{
		lgr << Log::t << pTrade->TradeDate << " " << pTrade->TradeTime << " "
			<< "#" << pTrade->OrderRef
			<< ((pTrade->Direction == THOST_FTDC_D_Buy) ? " Buy  " : " Sell ")
			<< (pTrade->Volume)
			<< " " << pTrade->InstrumentID << " "
			<< " @ " << Log::_2point << pTrade->Price
			<< pTrade->Volume * ((pTrade->Direction == THOST_FTDC_D_Buy) ? (MarketData->BidPrice1 - pTrade->Price) :
			(pTrade->Price - MarketData->AskPrice1)) << Log::endl;
		SleepFor(1000);
	}
}

void TdSpi::OnRspQryInvestorPositionDetail(
	CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	SleepFor(1000);
	ReqQryTrade(pInvestorPositionDetail->TradeID);
}

void TdSpi::OnRspQryInvestorPositionCombineDetail(
	CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	if (bIsLast)
	{
		//std::cout << pInvestorPositionCombineDetail->TradeID << Log::endl;
		pInvestorPositionCombineDetail->TradeID;
	}
}

void TdSpi::ReqQryInvestorPositionDetail()
{
	CThostFtdcQryInvestorPositionDetailField QryInvestorPositionDetail;
	strcpy(QryInvestorPositionDetail.BrokerID, BROKER);
	strcpy(QryInvestorPositionDetail.InstrumentID, ppInstrumentID[0]);
	strcpy(QryInvestorPositionDetail.InvestorID, USERID);
	SleepFor(1000);
	tdapi->ReqQryInvestorPositionDetail(&QryInvestorPositionDetail, ++nTdRequestID);
}

void TdSpi::ReqQryInvestorPositionCombineDetail()
{
	//CThostFtdcQryInvestorPositionCombineDetailField QryInvestorPositionCombineDetail;

}

void TdSpi::ReqQryTrade(const char* trade_id)
{
	CThostFtdcQryTradeField QryTrade;
	strcpy(QryTrade.BrokerID, BROKER);
	strcpy(QryTrade.ExchangeID, XCHGER);
	//strcpy(QryTrade.InstrumentID, ppInstrumentID[0]);
	strcpy(QryTrade.InvestorID, USERID);
	strcpy(QryTrade.TradeID, trade_id);
	tdapi->ReqQryTrade(&QryTrade, ++nTdRequestID);
}

void TdSpi::OnFrontDisconnected(int nReason)
{
	lgr << Log::t << "--->>> " << "OnFrontDisconnected" << Log::endl;
	lgr << Log::t << "--->>> Reason = " << nReason << Log::endl;
}

void TdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	lgr << Log::t << "--->>> " << "OnHeartBeatWarning" << Log::endl;
	lgr << Log::t << "--->>> nTimerLapse = " << nTimeLapse << Log::endl;
}

void TdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	IsErrorRspInfo(pRspInfo);
}

bool TdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// ���ErrorID != 0, ˵���յ��˴������Ӧ
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		lgr << Log::t << "[td]ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << Log::endl;
	return bResult;
}

bool TdSpi::IsMyOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->FrontID == FRONTID) &&
		(pOrder->SessionID == SESSION) &&
		(strcmp(pOrder->OrderRef, ORDER_REF) == 0));
}

bool TdSpi::IsTradingOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
		(pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
		(pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}
