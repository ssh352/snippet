/*************************************************************************
	> File Name: submarketdata_demo.cpp
	> Author: billowqiu
	> Mail: billowqiu@billowqiu.com 
	> Created Time: 2017-06-12 15:46:02
	> Last Changed: 2017-06-12 15:46:02
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

using namespace taf;

CThostFtdcMdApi* mdapi = nullptr;

//合约ID=>合约信息
typedef std::unordered_map<std::string, CThostFtdcInstrumentField> InstrumentMap;
InstrumentMap instruments_map;
bool batch_subscribe = true;
bool subscribe_all_instrument = true;
uint32_t batch_subscribe_size = 0;

//指定订阅的合约
typedef std::vector<std::string> InstrumentVec;
InstrumentVec instruments_vec;
//实际收到ctp订阅回应的合约id
std::vector<std::string> recv_sub_rsp_instrument;
std::set<std::string> recv_sub_rsp_instrument_set;
int32_t RspSubMarketDataCount = 0;
std::mutex m;
std::condition_variable cv;

taf::TC_RollLogger logger;
#define LOG_DEBUG logger.debug() << __LINE__ << "-" << std::this_thread::get_id() << "-"

class ThostFtdcMdCallback : public CThostFtdcMdSpi
{
private:
    CThostFtdcMdApi* mdapi_;
    int64_t requestid_;

public:
    ThostFtdcMdCallback(CThostFtdcMdApi* api)
    {
        mdapi_ = api;
        requestid_ = 0;
    }

	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected()
    {
        LOG_DEBUG << "OnFrontConnected()" << std::endl;
        std::cout << "OnFrontConnected()" << std::endl;
        CThostFtdcReqUserLoginField login_req;
        memset(&login_req, 0, sizeof(login_req));
        //memcpy(&login_req.BrokerID, "9999", 4);
        int ret = mdapi_->ReqUserLogin(&login_req, requestid_++);
        LOG_DEBUG << "ReqUserLogin ret: " << ret << std::endl;
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
        LOG_DEBUG << "OnFrontConnected ret: " << nReason << std::endl;
    }

	///心跳超时警告。当长时间未收到报文时，该方法被调用。
	///@param nTimeLapse 距离上次接收报文的时间
	virtual void OnHeartBeatWarning(int nTimeLapse)
    {
        LOG_DEBUG << "OnHeartBeatWarning nTimeLapse: " << nTimeLapse << std::endl;
    }

	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
        LOG_DEBUG << "OnRspUserLogin errorid: " << pRspInfo->ErrorID << ", errormsg: " << pRspInfo->ErrorMsg << std::endl;
        //登录成功，开始订阅行情
        mdapi_->SubscribeMarketData(NULL, 0);
        return;
        if (pRspInfo->ErrorID == 0)
        {
            if (subscribe_all_instrument)
            {
                const std::size_t instrumentids_length = instruments_map.size();
                LOG_DEBUG << "开始订阅合约个数: " << instrumentids_length << std::endl;
                //分批订阅
                char *instrumentids[batch_subscribe_size];
                std::size_t i = 0;
                std::size_t j = 0;
                for(auto instrument_field: instruments_map)
                {
                    j++;
                    instrumentids[i++] = (char*)(instrument_field.first).data();
                    //分批上限达到或者是最后一个合约了
                    if ((i >= batch_subscribe_size) || j == instruments_map.size())
                    {
                        int ret = mdapi_->SubscribeMarketData((char**)instrumentids, i);
                        LOG_DEBUG << "batch_subscribe: " << i << " SubscribeMarketData ret: " << ret << std::endl;
                        i = 0;
                    }
                    /*
                    else if (j == instruments_map.size())
                    {
                        instrumentids[i++] = (char*)(instrument_field.first).data();
                        int ret = mdapi_->SubscribeMarketData((char**)instrumentids, i);
                        LOG_DEBUG << "batch_subscribe: " << i << " SubscribeMarketData ret: " << ret << std::endl;
                    }
                    */
                }
                assert(j == instruments_map.size());
            }
            else
            {
                const std::size_t instrumentids_length = instruments_vec.size();
                LOG_DEBUG << "开始订阅合约个数: " << instrumentids_length << std::endl;
                char *instrumentids[instrumentids_length];
                std::size_t i = 0;
                for(auto instrument_field: instruments_vec)
                {
                    instrumentids[i] = (char*)instrument_field.data();
                    LOG_DEBUG << "订阅合约：" << instrumentids[i] << std::endl;
                    i++;
                }

                int ret = mdapi_->SubscribeMarketData((char**)instrumentids, instrumentids_length);
                LOG_DEBUG << "SubscribeMarketData ret: " << ret << std::endl;
            }

        }
    }

	///订阅行情应答
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
        if(!pSpecificInstrument)
        {
            LOG_DEBUG << pSpecificInstrument << "|" << pRspInfo << "|" << pRspInfo->ErrorID << "|" << pRspInfo->ErrorMsg << std::endl;
            return;
        }

        LOG_DEBUG << "订阅行情应答: " << pSpecificInstrument->InstrumentID << ", 当前订阅总数：" << ++RspSubMarketDataCount << ", isLast" << bIsLast << std::endl;
        if (pRspInfo->ErrorID)
        {
            logger.error() << "合约：" << pSpecificInstrument->InstrumentID << "订阅失败";
            return;
        }
        recv_sub_rsp_instrument.push_back(pSpecificInstrument->InstrumentID);
        recv_sub_rsp_instrument_set.insert(pSpecificInstrument->InstrumentID);
        if (bIsLast)
        {
            //比较发出订阅的合约和实际收到订阅应答的差集
            std::vector<std::string> send_sub_req_instrument;
            std::vector<std::string> diff;
            for(auto instrument_field: instruments_map)
            {
                send_sub_req_instrument.push_back(instrument_field.first);
            }
            //先排序
            std::sort(send_sub_req_instrument.begin(), send_sub_req_instrument.end());
            std::sort(recv_sub_rsp_instrument.begin(), recv_sub_rsp_instrument.end());

            std::set_difference(send_sub_req_instrument.begin(), send_sub_req_instrument.end(), 
                                recv_sub_rsp_instrument.begin(), recv_sub_rsp_instrument.end(), 
                                std::inserter(diff, diff.begin()));
            for(auto diff_id: diff)
            {
                LOG_DEBUG << "未收到应答的合约id: " << diff_id << std::endl;
            }
            LOG_DEBUG << "send size: " << send_sub_req_instrument.size() << ", recv size: " << recv_sub_rsp_instrument.size() << std::endl;
            LOG_DEBUG << recv_sub_rsp_instrument.size() << "|" << recv_sub_rsp_instrument_set.size() << std::endl;
        }
        //md_file.open(pSpecificInstrument->InstrumentID);
    }

	///登出请求响应
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
        std::cout << __FUNCTION__ << std::endl;
        std::cout << pRspInfo->ErrorID << ", " << pRspInfo->ErrorMsg << std::endl;
    }

	///错误应答
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
        std::cout << __FUNCTION__ << std::endl;
        std::cout << pRspInfo->ErrorID << ", " << pRspInfo->ErrorMsg << std::endl;
    }

	///深度行情通知
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
    {
        //md_file << *pDepthMarketData << std::endl;
    }

	///取消订阅行情应答
	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
        LOG_DEBUG << "OnRspUnSubMarketData instrumentid: " << pSpecificInstrument->InstrumentID << std::endl;
    }

	///订阅询价应答
	virtual void OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
        std::cout << __FUNCTION__ << std::endl;
    }

	///取消订阅询价应答
	virtual void OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
        std::cout << __FUNCTION__ << std::endl;
    }

	///询价通知
	virtual void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
    {
        std::cout << __FUNCTION__ << std::endl;
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
    //std::cout << fields.size() << std::endl;

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
    //std::cout << "index: " << i << std::endl;

    return 0;
}

int main(int argc, const char* argv[])
{
    if (argc < 4)
    {
        std::cout << "Usage: " << argv[0] << "md_ep(tcp://180.168.146.187:10031) instrumentids batch_subscribe_size" << std::endl;
        return 0;
    }
    std::string front_ep = argv[1];
    std::ifstream ifs(argv[2]);
    batch_subscribe_size = atoi(argv[3]);
    std::cout << "batch_subscribe_size: " << batch_subscribe_size << std::endl;

    logger.init("./submarketdata");

    //放到线程里面初始化
    auto md_func = [&front_ep, &ifs]() {
        //if (subscribe_all_instrument)
        {
            std::string instrument_str_in;
            CThostFtdcInstrumentField instrument_field;
            int i = 0;
            while(std::getline(ifs, instrument_str_in))
            {
                ParseFromString(&instrument_field, instrument_str_in);
                if (0 == strcmp(instrument_field.ExchangeID, "CZCE"))
                {
                    std::cout << instrument_field.InstrumentID << " " << ++i << std::endl;
                }
                instruments_map[instrument_field.InstrumentID] = instrument_field;
            }
            std::cout << "合约加载完毕，可以开始订阅行情了, 合约总数：" << instruments_map.size() << std::endl;
        }
        mdapi = CThostFtdcMdApi::CreateFtdcMdApi();
        if (NULL == mdapi)
        {
            std::cout << "create FtdcMdApi fail" << std::endl;
            return -1;
        }
        LOG_DEBUG << "md api version: " << mdapi->GetApiVersion() << std::endl;
        ThostFtdcMdCallback callback(mdapi);
        mdapi->RegisterSpi(&callback);
        mdapi->RegisterFront((char*)front_ep.data());
        mdapi->Init();
        LOG_DEBUG << "tradingday: " << mdapi->GetTradingDay() << std::endl;
        mdapi->Join();       
        LOG_DEBUG << "md api exit" << std::endl;
    };
    std::thread md_thread(md_func);

    std::cout << "ctp ctrl cmd: \n" 
              << "1: release mdapi \n"
              << "2: unsubscribe mdapi \n"
              << std::endl;
    while(true)
    {
        int select = 0;
        std::cin >> select;
        switch (select)
        {
            case 1:
                mdapi->Release();
                break;
            case 2:
                break;
        }
    }

    return 0;
}

