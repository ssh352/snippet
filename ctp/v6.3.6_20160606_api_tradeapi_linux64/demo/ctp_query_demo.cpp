/*************************************************************************
	> File Name: demo.cpp
	> Author: billowqiu
	> Mail: billowqiu@billowqiu.com 
	> Created Time: 2017-05-22 14:14:52
	> Last Changed: 2017-05-22 14:14:52
*************************************************************************/
#include "ThostFtdcMdApi.h"
#include "ThostFtdcTraderApi.h"
#include <stdint.h>
#include <string.h>
#include <iostream>
#include <thread>
#include <iterator> 
#include <unistd.h>
#include <fstream>
#include "util/tc_encoder.h"
#include "util/tc_common.h"
#include <unordered_map>
#include <set>
#include <mutex>
#include <typeinfo>
#include <condition_variable>
#include "util/tc_logger.h"
//#include "DataPersistence.h"

using namespace taf;

CThostFtdcTraderApi* traderapi =  nullptr;
int requestid = 0;

//合约ID=>合约信息
typedef std::unordered_map<std::string, CThostFtdcInstrumentField> InstrumentMap;
InstrumentMap instruments_map;
bool instruments_pulled = false;
bool subscribe_all_instrument = false;
//指定订阅的合约
typedef std::vector<std::string> InstrumentVec;
InstrumentVec instruments_vec;

//交易所=>产品代码
typedef std::unordered_map<std::string, std::set<std::string>> Exchange2ProductIDMap;
Exchange2ProductIDMap exchange2productid_map;

//实际收到ctp订阅回应的合约id
std::vector<std::string> recv_sub_rsp_instrument;
std::set<std::string> recv_sub_rsp_instrument_set;

std::ofstream exchange_file("exchange_info", ios::binary);

taf::TC_RollLogger logger;
#define LOG_DEBUG logger.debug() << __LINE__ << "-" << std::this_thread::get_id() << "-"

int32_t SerializeToString(const CThostFtdcInstrumentField& instrument, std::string* output);

std::ostream& operator<<(std::ostream& os, const CThostFtdcRspUserLoginField& data)
{
          os  << data.TradingDay << '\n'
              << data.SystemName << '\n'
              << data.FrontID << '\n'
              << data.SessionID << '\n';
}

std::ostream& operator<<(std::ostream& os, const CThostFtdcDepthMarketDataField& data)
{
      os  << "交易日:" << data.TradingDay << '|'
          << "合约代码:" << data.InstrumentID << '|'
          << "交易所代码:" << data.ExchangeID << '|'
          << "合约在交易所代码:" << data.ExchangeInstID << '|'
          << "最新价:" << data.LastPrice << '|'
          << "上次结算价:" << data.PreSettlementPrice << '|'
          << "昨收盘:" << data.PreClosePrice << '|'
          << "昨持仓量:" << data.PreOpenInterest << '|'
          << "今开盘:" << data.OpenPrice << '|'
          << "最高价:" << data.HighestPrice << '|'
          << "最低价:" << data.LowestPrice << '|'
          << "数量：" << data.Volume << '|'
          << "成交金额：" << data.Turnover << '|'
          << "持仓量：" << data.OpenInterest << '|'
          << "今收盘：" << data.ClosePrice << '|'
          << "本次结算价：" << data.SettlementPrice << '|'

          << "跌停板价:" << data.LowerLimitPrice << '|'
          << "涨停板价:" << data.UpperLimitPrice << '|' 

          << "昨虚实度：" << data.PreDelta << "|"
          << "今虚实度：" << data.CurrDelta << "|"

          << "最后修改时间：" << data.UpdateTime << "|"
          << "最后修改时间(MS)：" << data.UpdateMillisec << "|"

          << "申买价1：" << data.BidPrice1 << "|"
          << "申买量1：" << data.BidVolume1 << "|"
          << "申卖价1：" << data.AskPrice1 << "|"
          << "申卖量1：" << data.AskVolume1 << "|"

          << "申买价2：" << data.BidPrice2 << "|"
          << "申买量2：" << data.BidVolume2 << "|"
          << "申卖价2：" << data.AskPrice2 << "|"
          << "申卖量2：" << data.AskVolume2 << "|"

          << "申买价3：" << data.BidPrice3 << "|"
          << "申买量3：" << data.BidVolume3 << "|"
          << "申卖价3：" << data.AskPrice3 << "|"
          << "申卖量3：" << data.AskVolume3 << "|"

          << "申买价4：" << data.BidPrice4 << "|"
          << "申买量4：" << data.BidVolume4 << "|"
          << "申卖价4：" << data.AskPrice4 << "|"
          << "申卖量4：" << data.AskVolume4 << "|"

          << "申买价5：" << data.BidPrice5 << "|"
          << "申买量5：" << data.BidVolume5 << "|"
          << "申卖价5：" << data.AskPrice5 << "|"
          << "申卖量5：" << data.AskVolume5 << "|"

          << "当日均价：" << data.AveragePrice << "|"
          << "业务日期：" << data.ActionDay << "|";

    return os;
}

/*
///深度行情
struct CThostFtdcDepthMarketDataField
{
	///交易日
	TThostFtdcDateType	TradingDay;
	///合约代码
	TThostFtdcInstrumentIDType	InstrumentID;
	///交易所代码
	TThostFtdcExchangeIDType	ExchangeID;
	///合约在交易所的代码
	TThostFtdcExchangeInstIDType	ExchangeInstID;
	///最新价
	TThostFtdcPriceType	LastPrice;
	///上次结算价
	TThostFtdcPriceType	PreSettlementPrice;
	///昨收盘
	TThostFtdcPriceType	PreClosePrice;
	///昨持仓量
	TThostFtdcLargeVolumeType	PreOpenInterest;
	///今开盘
	TThostFtdcPriceType	OpenPrice;
	///最高价
	TThostFtdcPriceType	HighestPrice;
	///最低价
	TThostFtdcPriceType	LowestPrice;
	///数量
	TThostFtdcVolumeType	Volume;
	///成交金额
	TThostFtdcMoneyType	Turnover;
	///持仓量
	TThostFtdcLargeVolumeType	OpenInterest;
	///今收盘
	TThostFtdcPriceType	ClosePrice;
	///本次结算价
	TThostFtdcPriceType	SettlementPrice;
	///涨停板价
	TThostFtdcPriceType	UpperLimitPrice;
	///跌停板价
	TThostFtdcPriceType	LowerLimitPrice;
	///昨虚实度
	TThostFtdcRatioType	PreDelta;
	///今虚实度
	TThostFtdcRatioType	CurrDelta;
	///最后修改时间
	TThostFtdcTimeType	UpdateTime;
	///最后修改毫秒
	TThostFtdcMillisecType	UpdateMillisec;
	///申买价一
	TThostFtdcPriceType	BidPrice1;
	///申买量一
	TThostFtdcVolumeType	BidVolume1;
	///申卖价一
	TThostFtdcPriceType	AskPrice1;
	///申卖量一
	TThostFtdcVolumeType	AskVolume1;
	///当日均价
	TThostFtdcPriceType	AveragePrice;
	///业务日期
	TThostFtdcDateType	ActionDay;
};
*/
///////////////////////////////////////////////////
//交易
/*
///用户登录请求
struct CThostFtdcReqUserLoginField
{
	///交易日
	TThostFtdcDateType	TradingDay;
	///经纪公司代码
	TThostFtdcBrokerIDType	BrokerID;
	///用户代码
	TThostFtdcUserIDType	UserID;
	///密码
	TThostFtdcPasswordType	Password;
	///用户端产品信息
	TThostFtdcProductInfoType	UserProductInfo;
	///接口端产品信息
	TThostFtdcProductInfoType	InterfaceProductInfo;
	///协议信息
	TThostFtdcProtocolInfoType	ProtocolInfo;
	///Mac地址
	TThostFtdcMacAddressType	MacAddress;
	///动态密码
	TThostFtdcPasswordType	OneTimePassword;
	///终端IP地址
	TThostFtdcIPAddressType	ClientIPAddress;
	///登录备注
	TThostFtdcLoginRemarkType	LoginRemark;
};
*/
class ThostFtdcTraderCallback : public CThostFtdcTraderSpi
{
private:
    CThostFtdcTraderApi* api_;
public:
    ThostFtdcTraderCallback(CThostFtdcTraderApi* api)
    {
        api_ = api;
    }
	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected()
    {
        LOG_DEBUG << __FUNCTION__ << std::endl;
        CThostFtdcReqUserLoginField login_req;
        memset(&login_req, 0, sizeof(login_req));
        strcpy((char*)(&login_req.BrokerID), "9999");
        strcpy((char*)(&login_req.UserID), "092801");
        strcpy((char*)(&login_req.Password), "123456");

        int ret = api_->ReqUserLogin(&login_req, requestid++);
        LOG_DEBUG << "tarde ReqUserLogin ret: " << ret << std::endl;


    }
	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	///@param nReason 错误原因
	///        0x1001 网络读失败
	///        0x1002 网络写失败
	///        0x2001 接收心跳超时
	///        0x2002 发送心跳失败
	///        0x2003 收到错误报文
	virtual void OnFrontDisconnected(int nReason)
    {
        LOG_DEBUG << __FUNCTION__ << ", ret: " << nReason << std::endl;
    }
	///心跳超时警告。当长时间未收到报文时，该方法被调用。
	///@param nTimeLapse 距离上次接收报文的时间
	virtual void OnHeartBeatWarning(int nTimeLapse){};
	///客户端认证响应
	virtual void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
        LOG_DEBUG  << "threadid: " << std::this_thread::get_id()  << ", " << __FUNCTION__ << std::endl;
        LOG_DEBUG << "errorid: " << pRspInfo->ErrorID << ", errormsg: " << pRspInfo->ErrorMsg << std::endl;
        //登录成功
        if (pRspInfo->ErrorID == 0)
        {
            CThostFtdcQryInstrumentField qry_instrument;
            memset(&qry_instrument, 0, sizeof(qry_instrument));
            int ret = api_->ReqQryInstrument(&qry_instrument, requestid++);
            LOG_DEBUG << "tarde ReqQryInstrument ret: " << ret << std::endl;
        }
    }

	///登出请求响应
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///错误应答
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///报单通知
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder) {};

	///请求查询交易所响应
	virtual void OnRspQryExchange(CThostFtdcExchangeField *pExchange, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
        LOG_DEBUG << "OnRspQryExchange isLast: " << bIsLast << std::endl;
        std::string exchange_name = taf::TC_Encoder::gbk2utf8(pExchange->ExchangeName); 
        LOG_DEBUG << pExchange->ExchangeID << "|" << exchange_name << "|" << pExchange->ExchangeProperty << std::endl;
        //write to binary file
        exchange_file.write(reinterpret_cast<char*>(pExchange), sizeof(CThostFtdcExchangeField));
        if (bIsLast)
        {
            exchange_file.close();
        }
    }

	///请求查询产品响应
	virtual void OnRspQryProduct(CThostFtdcProductField *pProduct, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询合约响应
/*
struct CThostFtdcInstrumentField
{
	///合约代码
	TThostFtdcInstrumentIDType	InstrumentID;
	///交易所代码
	TThostFtdcExchangeIDType	ExchangeID;
	///合约名称
	TThostFtdcInstrumentNameType	InstrumentName;
	///合约在交易所的代码
	TThostFtdcExchangeInstIDType	ExchangeInstID;
	///产品代码
	TThostFtdcInstrumentIDType	ProductID;
	///产品类型
	TThostFtdcProductClassType	ProductClass;
	///交割年份
	TThostFtdcYearType	DeliveryYear;
	///交割月
	TThostFtdcMonthType	DeliveryMonth;
	///市价单最大下单量
	TThostFtdcVolumeType	MaxMarketOrderVolume;
	///市价单最小下单量
	TThostFtdcVolumeType	MinMarketOrderVolume;
	///限价单最大下单量
	TThostFtdcVolumeType	MaxLimitOrderVolume;
	///限价单最小下单量
	TThostFtdcVolumeType	MinLimitOrderVolume;
	///合约数量乘数
	TThostFtdcVolumeMultipleType	VolumeMultiple;
	///最小变动价位
	TThostFtdcPriceType	PriceTick;
	///创建日
	TThostFtdcDateType	CreateDate;
	///上市日
	TThostFtdcDateType	OpenDate;
	///到期日
	TThostFtdcDateType	ExpireDate;
	///开始交割日
	TThostFtdcDateType	StartDelivDate;
	///结束交割日
	TThostFtdcDateType	EndDelivDate;
	///合约生命周期状态
	TThostFtdcInstLifePhaseType	InstLifePhase;
	///当前是否交易
	TThostFtdcBoolType	IsTrading;
	///持仓类型
	TThostFtdcPositionTypeType	PositionType;
	///持仓日期类型
	TThostFtdcPositionDateTypeType	PositionDateType;
	///多头保证金率
	TThostFtdcRatioType	LongMarginRatio;
	///空头保证金率
	TThostFtdcRatioType	ShortMarginRatio;
	///是否使用大额单边保证金算法
	TThostFtdcMaxMarginSideAlgorithmType	MaxMarginSideAlgorithm;
	///基础商品代码
	TThostFtdcInstrumentIDType	UnderlyingInstrID;
	///执行价
	TThostFtdcPriceType	StrikePrice;
	///期权类型
	TThostFtdcOptionsTypeType	OptionsType;
	///合约基础商品乘数
	TThostFtdcUnderlyingMultipleType	UnderlyingMultiple;
	///组合类型
	TThostFtdcCombinationTypeType	CombinationType;
}; */
    virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, 
                                    int nRequestID, bool bIsLast) 
    { 
        std::string instrumentname = taf::TC_Encoder::gbk2utf8(pInstrument->InstrumentName); 
        /* 
        ofs << "合约代码:" << pInstrument->InstrumentID 
            << ", 交易所代码：" << pInstrument->ExchangeID 
            << ", 合约名称：" << instrumentname 
            << ", 合约在交易所代码：" << pInstrument->ExchangeInstID 
            << "，产品代码：" << pInstrument->ProductID 
            << "，产品类型：" << pInstrument->ProductClass 
            << "，交割年份：" << pInstrument->DeliveryYear
            << "，交割月份：" << pInstrument->DeliveryMonth
            << "，创建日：" << pInstrument->CreateDate
            << "，上市日：" << pInstrument->OpenDate
            << "，到期日：" << pInstrument->ExpireDate
            << "，开始交割日：" << pInstrument->StartDelivDate
            << "，结束交割日：" << pInstrument->EndDelivDate
            << "，当前是否交易：" << pInstrument->IsTrading
            << "，基础商品代码：" << pInstrument->UnderlyingInstrID
            << std::endl; 
        */
        std::string instrument;
        SerializeToString(*pInstrument, &instrument);

        instruments_map[pInstrument->InstrumentID] = *pInstrument;
        exchange2productid_map[pInstrument->ExchangeID].insert(pInstrument->ProductID);

        if (bIsLast)
        {
            LOG_DEBUG << "合约拉取完毕，可以开始订阅行情了, 合约总数：" << instruments_map.size() << std::endl;
            instruments_pulled = true;
            for(auto exchange2productid_item : exchange2productid_map) 
            {
                LOG_DEBUG << "交易所：" << exchange2productid_item.first << "---------";
                for(auto productid: exchange2productid_item.second)
                {
                    LOG_DEBUG << productid;
                    LOG_DEBUG << "|";
                }
                LOG_DEBUG << endl;
            }
        }
    }

};

    int32_t SerializeToString(const CThostFtdcInstrumentField& instrument, std::string* output)
    {
        std::string instrumentname = taf::TC_Encoder::gbk2utf8(instrument.InstrumentName); 
        std::stringstream ss;

        /*
        ss << typeid(instrument.InstrumentID).name() << "|"
           << typeid(instrument.ExchangeID).name() << "|"
           << typeid(instrument.InstrumentName).name() << "|"
           << typeid(instrument.ExchangeInstID).name() << "|"
           << typeid(instrument.ProductID).name() << "|"
           << typeid(instrument.ProductClass).name() << "|"
           << typeid(instrument.DeliveryYear).name() << "|"
           << typeid(instrument.DeliveryMonth).name() << "|"
           << typeid(instrument.MaxMarketOrderVolume).name() << "|"
           << typeid(instrument.MinMarketOrderVolume).name() << "|"
           << typeid(instrument.MaxLimitOrderVolume).name() << "|"
           << typeid(instrument.MinLimitOrderVolume).name() << "|"
           << typeid(instrument.VolumeMultiple).name() << "|"
           << typeid(instrument.PriceTick).name() << "|"
           << typeid(instrument.CreateDate).name() << "|"
           << typeid(instrument.OpenDate).name() << "|"
           << typeid(instrument.ExpireDate).name() << "|"
           << typeid(instrument.StartDelivDate).name() << "|"
           << typeid(instrument.EndDelivDate).name() << "|"
           << typeid(instrument.InstLifePhase).name() << "|"
           << typeid(instrument.IsTrading).name() << "|"
           << typeid(instrument.PositionType).name() << "|"
           << typeid(instrument.PositionDateType).name() << "|"
           << typeid(instrument.LongMarginRatio).name() << "|"
           << typeid(instrument.ShortMarginRatio).name() << "|"
           << typeid(instrument.MaxMarginSideAlgorithm).name() << "|"
           << typeid(instrument.UnderlyingInstrID).name() << "|"
           << typeid(instrument.StrikePrice).name() << "|"
           << typeid(int(instrument.OptionsType)).name() << "|"
           << typeid(instrument.UnderlyingMultiple).name() << "|"
           << typeid(instrument.CombinationType).name();

           */
        ss << instrument.InstrumentID << "|"
           << instrument.ExchangeID << "|"
           << instrument.InstrumentName << "|"
           << instrument.ExchangeInstID << "|"
           << instrument.ProductID << "|"
           << instrument.ProductClass << "|"
           << instrument.DeliveryYear << "|"
           << instrument.DeliveryMonth << "|"
           << instrument.MaxMarketOrderVolume << "|"
           << instrument.MinMarketOrderVolume << "|"
           << instrument.MaxLimitOrderVolume << "|"
           << instrument.MinLimitOrderVolume << "|"
           << instrument.VolumeMultiple << "|"
           << instrument.PriceTick << "|"
           << instrument.CreateDate << "|"
           << instrument.OpenDate << "|"
           << instrument.ExpireDate << "|"
           << instrument.StartDelivDate << "|"
           << instrument.EndDelivDate << "|"
           << instrument.InstLifePhase << "|"
           << instrument.IsTrading << "|"

           << instrument.PositionType << "|"
           << instrument.PositionDateType << "|"

           << instrument.LongMarginRatio << "|"
           << instrument.ShortMarginRatio << "|"
           
           << instrument.MaxMarginSideAlgorithm << "|"
           << instrument.UnderlyingInstrID << "|"
           << instrument.StrikePrice << "|"
           //CTP返回的这个字段不正确，是数字0，不是'1','2'
           << int(instrument.OptionsType) << "|"
           << instrument.UnderlyingMultiple << "|"
           << instrument.CombinationType;

        *output = ss.str();

        return 0;
    }
    //A31_c|A9_c|A21_c|A31_c|A31_c|c|i|i|i|i|i|i|i|d|A9_c|A9_c|A9_c|A9_c|A9_c|c|i|c|c|d|d|c|A31_c|d|i|d|c

    int32_t ParseFromString(CThostFtdcInstrumentField* instrument, const std::string& input)
    {
        int i = 0;
        std::vector<std::string> fields = taf::TC_Common::sepstr<std::string>(input, "|", true);
        LOG_DEBUG << fields.size() << std::endl;

        //SR807|CZCE|白砂糖807|SR807|SR|1|2018|7|200|1|1000|1|10|1|20161215|20170117|20180713|20180716|20180717|1|1|2|2|0.05|0.05|0||1.79769e+308|0|1|0
        memset(instrument->InstrumentID, 0, 31);
        strncpy(instrument->InstrumentID, fields[i].c_str(), fields[i].size());
        i++;


        //A31_c|A9_c|A21_c|A31_c|A31_c|c|i|i|i|i|i|i|i|d|A9_c|A9_c|A9_c|A9_c|A9_c|c|i|c|c|d|d|c|A31_c|d|i|d|c
        memset(instrument->ExchangeID, 0, 9);
        strncpy(instrument->ExchangeID, fields[i].c_str(), fields[i].size());
        i++;

        memset(instrument->InstrumentName, 0, 21);
        strncpy(instrument->InstrumentName, fields[i].c_str(), fields[i].size());
        i++;

        memset(instrument->ExchangeInstID, 0, 31);
        strncpy(instrument->ExchangeInstID, fields[i].c_str(), fields[i].size());
        i++;

        memset(instrument->ProductID, 0, 31);
        strncpy(instrument->ProductID, fields[i].c_str(), fields[i].size());
        i++;
        
        instrument->ProductClass = fields[i][0];
        i++;

        instrument->DeliveryYear = TC_Common::strto<int>(fields[i]);
        i++;
        instrument->DeliveryMonth = TC_Common::strto<int>(fields[i]);
        i++;
        instrument->MaxMarketOrderVolume = TC_Common::strto<int>(fields[i]);
        i++;
        instrument->MinMarketOrderVolume = TC_Common::strto<int>(fields[i]);
        i++;
        instrument->MaxLimitOrderVolume = TC_Common::strto<int>(fields[i]);
        i++;
        instrument->MinLimitOrderVolume = TC_Common::strto<int>(fields[i]);
        i++;
        instrument->VolumeMultiple = TC_Common::strto<int>(fields[i]);
        i++;

        instrument->PriceTick = TC_Common::strto<double>(fields[i]);
        i++;

        memset(instrument->CreateDate, 0, 9);
        strncpy(instrument->CreateDate, fields[i].c_str(), fields[i].size());
        i++;

        memset(instrument->OpenDate, 0, 9);
        strncpy(instrument->OpenDate, fields[i].c_str(), fields[i].size());
        i++;

        memset(instrument->ExpireDate, 0, 9);
        strncpy(instrument->ExpireDate, fields[i].c_str(), fields[i].size());
        i++;

        memset(instrument->StartDelivDate, 0, 9);
        strncpy(instrument->StartDelivDate, fields[i].c_str(), fields[i].size());
        i++;

        memset(instrument->EndDelivDate, 0, 9);
        strncpy(instrument->EndDelivDate, fields[i].c_str(), fields[i].size());
        i++;

        instrument->InstLifePhase = fields[i][0];
        i++;

    //A31_c|A9_c|A21_c|A31_c|A31_c|c|i|i|i|i|i|i|i|d|A9_c|A9_c|A9_c|A9_c|A9_c|c|i|c|c|d|d|c|A31_c|d|i|d|c
        instrument->IsTrading = TC_Common::strto<int>(fields[i]);
        i++;
        instrument->PositionType = fields[i][0];
        i++;
        instrument->PositionDateType = fields[i][0];
        i++;

        instrument->LongMarginRatio = TC_Common::strto<double>(fields[i]);
        i++;
        instrument->ShortMarginRatio = TC_Common::strto<double>(fields[i]);
        i++;

        instrument->MaxMarginSideAlgorithm = fields[i][0];
        i++;
        //基础商品代码为空
        memset(instrument->UnderlyingInstrID, 0, 9);
        i++;

        instrument->StrikePrice = TC_Common::strto<double>(fields[i]);
        i++;

        //instrument->OptionsType = TC_Common::strto<int>(fields[i]);
        instrument->OptionsType = fields[i][0]-'0';
        i++;

        instrument->UnderlyingMultiple = TC_Common::strto<double>(fields[i]);
        i++;

        instrument->CombinationType = fields[i][0];
        LOG_DEBUG << "index: " << i << std::endl;

        return 0;
    }

int main(int argc, const char* argv[])
{
    if (argc < 3)
    {
        LOG_DEBUG << "Usage: " << argv[0] << " tarde_ep(tcp://180.168.146.187:10030) instrumentids" << std::endl;
        return 0;
    }
    std::string tarde_ep = argv[1];
    std::string instrumentids = argv[2];

    if (instrumentids == "0")
    {
        LOG_DEBUG << "订阅所有行情" << std::endl;
        subscribe_all_instrument = true;
    }
    else
    {
        LOG_DEBUG << "订阅指定合约行情: " << instrumentids << std::endl;
        instruments_vec = taf::TC_Common::sepstr<std::string>(instrumentids, ":");
    }

    logger.init("./ctp_query_demo");
    //先拉取所有合约
    if (subscribe_all_instrument)
    {
        auto trader_func = [&tarde_ep]() {
            traderapi = CThostFtdcTraderApi::CreateFtdcTraderApi();
            ThostFtdcTraderCallback trader_callback(traderapi);
            traderapi->RegisterSpi(&trader_callback);
            traderapi->RegisterFront((char*)tarde_ep.data());
            traderapi->Init();
            traderapi->Join();
            LOG_DEBUG << std::this_thread::get_id() << " trade api exit" << std::endl;
        };
        std::thread tarder_thread(trader_func);
        tarder_thread.detach();
    }

    std::cout << "ctp ctrl cmd: \n" 
              << "1: query exchange\n"
              << "2: read exchang\n"
              << "3: query instrument\n"
              << std::endl;

    std::ifstream ifs_exchange_file("exchange_info", ios::binary);
    while(true)
    {
        int select = 0;
        std::cin >> select;
        switch (select)
        {
            case 1:
                CThostFtdcQryExchangeField qry_exchange;
                memset(&qry_exchange, 0, sizeof(CThostFtdcQryExchangeField));
                traderapi->ReqQryExchange(&qry_exchange, requestid++);
                break;
            case 2:
                CThostFtdcExchangeField  exchange_info;
                while(ifs_exchange_file.read(reinterpret_cast<char*>(&exchange_info), sizeof(CThostFtdcExchangeField)))
                {
                    LOG_DEBUG << exchange_info.ExchangeID << "|" << taf::TC_Encoder::gbk2utf8(exchange_info.ExchangeName) << "|" << exchange_info.ExchangeProperty << std::endl;
                }
                break;

            case 3:
                {
                    CThostFtdcQryInstrumentField qry_instrument;
                    //查询所有合约信息
                    memset(&qry_instrument, 0, sizeof(qry_instrument));
                    int ret = traderapi->ReqQryInstrument(&qry_instrument, requestid++);
                    LOG_DEBUG << "tarde ReqQryInstrument ret: " << ret << std::endl;
                    break;
                }
        }
    }

    return 0;
}

