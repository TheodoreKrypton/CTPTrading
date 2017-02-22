#include "EA.h"
#include "Common.h"
#include "Trade.h"

void EA()
{
	int tmp_direction = THOST_FTDC_D_Buy;
	double lastp = MarketData->AskPrice1;
	int holding = 0;
	int lot = 1;
	OrderSend(ppInstrumentID[0], THOST_FTDC_OF_Open, tmp_direction, lot);
	double interval = 16;
	int tp = 6;
	int layer = 0;
	double mul = 1.6;
	double loss = 0;
	while (true)
	{
		if (tmp_direction == THOST_FTDC_D_Buy)
		{
			if (MarketData->BidPrice1 < lastp - interval)
			{
				// Ö¹ËðÂò£¬·´ÊÖÂô
				OrderSend(ppInstrumentID[0], THOST_FTDC_OF_Close, THOST_FTDC_D_Sell, lot);
				lot = pow(mul, ++layer);
				loss += interval * lot;
				tmp_direction = THOST_FTDC_D_Sell;
				OrderSend(ppInstrumentID[0], THOST_FTDC_OF_Open, THOST_FTDC_D_Sell, lot);
			}
			else if (MarketData->BidPrice1 - lastp > loss / lot + tp)
			{
				OrderSend(ppInstrumentID[0], THOST_FTDC_OF_Close, THOST_FTDC_D_Sell, lot);
				lot = 1;
				loss = 0;
				tmp_direction = THOST_FTDC_D_Buy;
				OrderSend(ppInstrumentID[0], THOST_FTDC_OF_Open, THOST_FTDC_D_Buy, lot);
			}
		}
		else if (tmp_direction == THOST_FTDC_D_Sell)
		{
			if (MarketData->AskPrice1 > lastp + interval)
			{
				OrderSend(ppInstrumentID[0], THOST_FTDC_OF_Close, THOST_FTDC_D_Buy, lot);
				lot = pow(mul, ++layer);
				loss += interval * lot;
				tmp_direction = THOST_FTDC_D_Buy;
				OrderSend(ppInstrumentID[0], THOST_FTDC_OF_Open, THOST_FTDC_D_Buy, lot);
			}
			else if (lastp - MarketData->AskPrice1 > loss / lot + tp)
			{
				OrderSend(ppInstrumentID[0], THOST_FTDC_OF_Close, THOST_FTDC_D_Buy, lot);
				lot = 1;
				loss = 0;
				tmp_direction = THOST_FTDC_D_Buy;
				OrderSend(ppInstrumentID[0], THOST_FTDC_OF_Open, THOST_FTDC_D_Buy, lot);
			}
		}
	}
}