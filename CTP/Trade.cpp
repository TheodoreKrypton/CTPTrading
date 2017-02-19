#define _CRT_SECURE_NO_WARNINGS

#include "Trade.h"
#include "TraderSpi.h"

#include <iostream>

void InitTrade()
{
	system("md .\\tdflow");
	tdapi = CThostFtdcTraderApi::CreateFtdcTraderApi("./tdflow/");
	tdspi = new TdSpi;
	tdapi->RegisterSpi(tdspi);
	tdapi->SubscribePublicTopic(THOST_TE_RESUME_TYPE::THOST_TERT_QUICK);
	tdapi->SubscribePrivateTopic(THOST_TE_RESUME_TYPE::THOST_TERT_QUICK);
	tdapi->RegisterFront(FRONTADD_TD);
	tdapi->Init();
}

void OrderSend(const char* instrument_id, char type, char direction, int volume)
{
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///���͹�˾����
	strcpy(req.BrokerID, BROKER);
	///Ͷ���ߴ���
	strcpy(req.InvestorID, USERID);
	///��Լ����
	strcpy(req.InstrumentID, instrument_id);
	///��������
	//strcpy(req.OrderRef, ORDER_REF);
	///�û�����
	//	TThostFtdcUserIDType	UserID;
	///�����۸�����: �޼�
	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	///��������: 
	req.Direction = direction;
	///��Ͽ�ƽ��־: ����
	req.CombOffsetFlag[0] = type;
	///���Ͷ���ױ���־
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	///�۸�
	req.LimitPrice = (direction == THOST_FTDC_D_Buy) ? MarketData->AskPrice1 : MarketData->BidPrice1;
	///����: 1
	req.VolumeTotalOriginal = volume;
	///��Ч������: ������Ч
	req.TimeCondition = THOST_FTDC_TC_GFD;
	///GTD����
	//	TThostFtdcDateType	GTDDate;
	///�ɽ�������: �κ�����
	req.VolumeCondition = THOST_FTDC_VC_AV;
	///��С�ɽ���: 1
	req.MinVolume = 1;
	///��������: ����
	req.ContingentCondition = THOST_FTDC_CC_Immediately;
	///ֹ���
	//req.StopPrice = stop_price;
	///ǿƽԭ��: ��ǿƽ
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	///�Զ������־: ��
	req.IsAutoSuspend = 0;
	///ҵ��Ԫ
	//	TThostFtdcBusinessUnitType	BusinessUnit;
	///������
	//	TThostFtdcRequestIDType	RequestID;
	///�û�ǿ����־: ��
	req.UserForceClose = 0;

	int iResult = tdapi->ReqOrderInsert(&req, ++nTdRequestID);
	lgr << Log::t << "[td]Requst Order Insert... " << ((iResult == 0) ? "Successed" : "Failed") << Log::endl;
}

void DisplayOrders()
{
	tdspi->ReqQryInvestorPositionDetail();
}