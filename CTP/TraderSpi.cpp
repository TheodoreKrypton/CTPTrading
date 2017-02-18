#define _CRT_SECURE_NO_WARNINGS

#include "TraderSpi.h"
#include <iostream>
#include <iomanip>

// �����ж�
bool IsFlowControl(int iResult)
{
	return ((iResult == -2) || (iResult == -3));
}

void TdSpi::OnFrontConnected()
{
	std::cerr << "[td]Front Connected..." << std::endl;
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
	std::cerr << "[td]Send Login Request... " << ((iResult == 0) ? "Successed" : "Failed") << std::endl;
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
		std::cerr << "[td]Date: " << tdapi->GetTradingDay() << std::endl;
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
	std::cerr << "[td]Request Settlement Info Confirm... " << ((iResult == 0) ? "Successed" : "Failed") << std::endl;
}

void TdSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		tdlogin = true;
		///�����ѯ��Լ
		std::cerr << "[td]Instruments:" << std::endl;
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
			std::cerr << "[td]Request Query Instrument... " << ((iResult == 0) ? "Successed" : "Failed") << std::endl;
			break;
		}
		else
		{
			std::cerr << "[td]Request Query Instrument... " << "Flow Control" << std::endl;
		}
	} // while
}

void TdSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		std::cerr << "[td]Instrument Id:\t" << pInstrument->InstrumentID << std::endl;
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
			std::cerr << "[td]Request Query Trading Account... " << ((iResult == 0) ? "Successed" : "Failed") << std::endl;
			break;
		}
		else
		{
			std::cerr << "[td]Request Query Trading Account... " << "Flow Control" << std::endl;
		}
	} // while
}

void TdSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		std::cerr << std::fixed << std::setprecision(2) << "[td]Balance: " << pTradingAccount->Balance << std::endl;
		std::cerr << std::fixed << std::setprecision(2) << "[td]Available: " << pTradingAccount->Available << std::endl;
		std::cerr << std::fixed << std::setprecision(2) << "[td]Close Profit: " << pTradingAccount->CloseProfit << std::endl;
		std::cerr << std::fixed << std::setprecision(2) << "[td]Position Profit: " << pTradingAccount->PositionProfit << std::endl;
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
			std::cerr << "[td]Request Query Investor Position... " << ((iResult == 0) ? "Successed" : "Failed") << std::endl;
			break;
		}
		else
		{
			std::cerr << "[td]Request Query Investor Position... " << "Flow Control" << std::endl;
		}
	} // while
}

void TdSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		if (pInvestorPosition)
		{
			std::cerr << "[td]Open Volume:" << pInvestorPosition->OpenVolume << std::endl;
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
//	std::cerr << "[td]Requst Order Insert... " << ((iResult == 0) ? "Successed" : "Failed") << std::endl;
//}

void TdSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cerr << "[td]Order: #" << pInputOrder->OrderRef << std::endl;
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
	std::cerr << "--->>> ������������: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << std::endl;
	ORDER_ACTION_SENT = true;
}

void TdSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	std::cerr << "--->>> " << "OnRspOrderAction" << std::endl;
	IsErrorRspInfo(pRspInfo);
}

///����֪ͨ
void TdSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	std::cerr << "[td]Direction: " << (pOrder->Direction == '0' ? "Buy" : "Sell")
		<< ", Price: " << pOrder->LimitPrice
		<< ", Status: " << pOrder->StatusMsg << std::endl;
}

///�ɽ�֪ͨ
void TdSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	std::cerr << "[td]" << pTrade->InstrumentID
		<< " At Price: " << pTrade->Price << ", " 
		<< pTrade->Volume << " Traded" << std::endl;
}

void TdSpi::OnRspQryTrade(
	CThostFtdcTradeField *pTrade,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	if (bIsLast)
	{
		std::cerr << pTrade->TradeDate << " " << pTrade->TradeTime << " "
			<< "#" << pTrade->OrderRef
			<< ((pTrade->Direction == THOST_FTDC_D_Buy) ? " Buy  " : " Sell ")
			<< (pTrade->Volume)
			<< " " << pTrade->InstrumentID << " "
			<< " @ " << std::fixed << std::setprecision(2) << pTrade->Price
			<< pTrade->Volume * ((pTrade->Direction == THOST_FTDC_D_Buy) ? (MarketData->BidPrice1 - pTrade->Price) :
			(pTrade->Price - MarketData->AskPrice1)) << std::endl;
	}
}

void TdSpi::OnRspQryInvestorPositionDetail(
	CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast)
{
	ReqQryTrade(pInvestorPositionDetail->TradeID);
}

void TdSpi::OnRspQryInvestorPositionCombineDetail(
	CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, bool bIsLast)
{
	if (bIsLast)
	{
		//std::cout << pInvestorPositionCombineDetail->TradeID << std::endl;
		pInvestorPositionCombineDetail->TradeID;
	}
}

void TdSpi::ReqQryInvestorPositionDetail()
{
	CThostFtdcQryInvestorPositionDetailField QryInvestorPositionDetail;
	strcpy(QryInvestorPositionDetail.BrokerID, BROKER);
	strcpy(QryInvestorPositionDetail.InstrumentID, ppInstrumentID[0]);
	strcpy(QryInvestorPositionDetail.InvestorID, USERID);
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
	std::cerr << "--->>> " << "OnFrontDisconnected" << std::endl;
	std::cerr << "--->>> Reason = " << nReason << std::endl;
}

void TdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	std::cerr << "--->>> " << "OnHeartBeatWarning" << std::endl;
	std::cerr << "--->>> nTimerLapse = " << nTimeLapse << std::endl;
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
		std::cerr << "[td]ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
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
