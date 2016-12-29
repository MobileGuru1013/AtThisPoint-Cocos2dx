
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "GameController.h"
#include "GameScene_Online.h"

#include "common.h"

static std::mutex       g_sendQueueMutex;
static std::mutex       g_responseQueueMutex;
static std::mutex       g_responseMsgMutex;

static std::mutex       g_SleepMutex;
static std::condition_variable      g_SleepCondition;

static bool g_need_quit = false;

static std::queue<std::string> g_responseQueue;
static std::queue<std::string> g_sendQueue;

static std::string g_responseMsg = "";

GameController* GameController::_instance = NULL;

GameController::GameController()
{
}

GameController::~GameController()
{
}

GameController* GameController::getInstance()
{
    if(_instance == NULL) {
        
        _instance = new GameController();
        _instance->isProcessing = false;
        _instance->isPause = false;
        _instance->_viewlistener = NULL;
    }
    return _instance;
}

void GameController::setViewListner(GameScene_Online *listener)
{
    _viewlistener = listener;
}

//request thread
void GameController::startSendThread()
{
    GameController* pController = GameController::getInstance();
    
    g_need_quit = false;
    auto t = std::thread(CC_CALLBACK_0(GameController::senderThread, pController));
    t.detach();
}



void GameController::senderThread()
{
    while (true)
    {
        if (g_need_quit)
        {
            break;
        }
        
        if (g_sendQueue.empty() == true) {
            
            sleep(1);
            continue;
        }
        
        std::string msg = "";
        g_sendQueueMutex.lock();
        
        msg = g_sendQueue.front();
        g_sendQueue.pop();
        
        g_sendQueueMutex.unlock();
        
        warpClientRef->sendChat(msg);
       
        //CCLOG("senderThread : %s\n", msg.c_str());
    }
    
    g_sendQueueMutex.lock();
    
    for (int i = 0; i < g_responseQueue.size(); i++) {
        
        g_sendQueue.pop();
    }
    
    g_sendQueueMutex.unlock();
}

//start respond thread
void GameController::startRespondThread()
{
    GameController* pController = GameController::getInstance();
    
    g_need_quit = false;
    auto t = std::thread(CC_CALLBACK_0(GameController::receiverThread, pController));
    t.detach();
}

//stop respond thread
void GameController::stopRespondThread()
{
    _viewlistener = NULL;
    isProcessing = false;
    g_need_quit = true;
}

void GameController::receiverThread()
{
    auto scheduler = cocos2d::Director::getInstance()->getScheduler();
    
    while (true)
    {
        if (g_need_quit)
        {
            break;
        }
        
        if (isProcessing == true || g_responseQueue.empty() == true) {
            
            sleep(1);
            continue;
        }
        
        isProcessing = true;
        std::string msg = "";
        g_responseQueueMutex.lock();
        
        msg = g_responseQueue.front();
        g_responseQueue.pop();
                
        g_responseQueueMutex.unlock();
        
        g_responseMsgMutex.lock();
        
        g_responseMsg = msg;
        
        g_responseMsgMutex.unlock();
        
        //CCLOG("receiverThread : %s\n", msg.c_str());
        scheduler->performFunctionInCocosThread(CC_CALLBACK_0(GameController::dispatchResponseCallbacks, this));
    }
    
    g_responseQueueMutex.lock();
    
    g_responseQueue = std::queue<std::string>();
    
    g_responseQueueMutex.unlock();
}

// Poll and notify main thread if responses exists in queue
void GameController::dispatchResponseCallbacks()
{
    if (_viewlistener == NULL) {
        
        return;
    }
    
    //occurs when cocos thread fires but the network thread has already quited
    g_responseMsgMutex.lock();
    std::string msg = g_responseMsg;
    g_responseMsgMutex.unlock();
    
    _viewlistener->onChatReceived(msg);
}

void GameController::connect2AppWarp()
{
    //if (g_isFirstLaunch)
    {
        //g_isFirstLaunch = !g_isFirstLaunch;
        AppWarp::Client::initialize(APPWARP_APP_KEY, APPWARP_HOSTNAME);
    }
    
    isPause = false;
    isProcessing = false;
    isRoomCreated = false;
    
    warpClientRef = AppWarp::Client::getInstance();
    //warpClientRef->setRecoveryAllowance(60);
    warpClientRef->setConnectionRequestListener(this);
    warpClientRef->setNotificationListener(this);
    warpClientRef->setRoomRequestListener(this);
    warpClientRef->setZoneRequestListener(this);
    warpClientRef->setTurnBasedRoomRequestListener(this);
    
    
    std::string authData = StringUtils::format("%s,%s,%d,%.0f,%d", g_mainUserInfo.user_name.c_str(), g_mainUserInfo.photo_id.c_str(), g_mainUserInfo.level, g_mainUserInfo.points,  g_nTablePos);
    warpClientRef->connect(g_mainUserInfo.user_id, authData);
    
    startRespondThread();
    //startSendThread();
}

void GameController::disconnect2AppWarp()
{
    stopRespondThread();
    
    _viewlistener = NULL;
    
    if (warpClientRef == NULL) {
        
        return;
    }

    //g_SleepCondition.notify_one();
    
    //if you already join room, exit in room
    if (g_roomId.length() > 0) {
        
        warpClientRef->leaveRoom(g_roomId);
        warpClientRef->unsubscribeRoom(g_roomId);
    }
    warpClientRef->setTurnBasedRoomRequestListener(NULL);
    warpClientRef->setRoomRequestListener(NULL);
    
    warpClientRef->disconnect();
}

void GameController::joinToGameRoom()
{
    //warpClientRef->joinRoomInUserRange(1, 3, false);
    if(g_nRoomType == FREE_ROOM) {
    
        std::string strType = "0";
        std::string strBuyinkey = StringUtils::format("%.0f", tmp_roomBuyin_key);
        std::string strBuyin = StringUtils::format("%.0f", g_nBuyin);
        std::string strFee = StringUtils::format("%d", g_nPlayFee);
        
        std::map<std::string, std::string> table;
        table["type"] = strType;
        table["buyin"] = strBuyinkey;
        table["fee"] = strFee;
       
        warpClientRef->joinRoomWithProperties(table);
    }
    else if(g_nRoomType == MASTER_ROOM) {
        
        time_t     now = time(0);
        struct tm  tstruct;
        char       buf[80];
        tstruct = *localtime(&now);
        strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
        std::string room_name = buf;
        
        std::string strType = "1";
        std::string strBuyin = StringUtils::format("%.0f", g_nBuyin);
        std::string strFee = StringUtils::format("%d", g_nPlayFee);
        
        std::map<std::string, std::string> table;
        table["type"] = strType;
        table["buyin"] = strBuyin;
        table["fee"] = strFee;
        
        std::string room_owner = "tian";
        warpClientRef->createTurnRoom(room_name, room_owner, MAX_PLAYERS, table, 120);
    }
    else if(g_nRoomType == VIP_ROOM) { 
        
        warpClientRef->joinRoom(g_roomId);
    }
}

void GameController::leaveGameRoom()
{
    warpClientRef->leaveRoom(g_roomId);
}

void GameController::sendChatMessage(std::string msg)
{
    CCLOG("sendChatMessage : %s\n", msg.c_str());
    
    warpClientRef->sendChat(msg);
    
//    g_sendQueueMutex.lock();
//    
//    g_sendQueue.push(msg);
//    
//    g_sendQueueMutex.unlock();
}

void GameController::showWarningMsg()
{
    GameController* pController = GameController::getInstance();
    
    auto t = std::thread(CC_CALLBACK_0(GameController::showWarningThread, pController));
    t.detach();
}

void GameController::hideWarningMsg()
{
    GameController* pController = GameController::getInstance();
    
    auto t = std::thread(CC_CALLBACK_0(GameController::hideWarningThread, pController));
    t.detach();
}

void GameController::showErrorMsg()
{
    GameController* pController = GameController::getInstance();
    
    auto t = std::thread(CC_CALLBACK_0(GameController::showErrorThread, pController));
    t.detach();
}

void GameController::showWarningThread()
{
    auto scheduler = cocos2d::Director::getInstance()->getScheduler();
    scheduler->performFunctionInCocosThread([](){
        
        if (GameController::getInstance()->_viewlistener == NULL) {
            
            return;
        }
        GameController::getInstance()->_viewlistener->showAlert(g_msgWarning);
    });
}

void GameController::showErrorThread()
{
    auto scheduler = cocos2d::Director::getInstance()->getScheduler();
    scheduler->performFunctionInCocosThread([](){
        
        if (GameController::getInstance()->_viewlistener == NULL) {
            
            return;
        }
        GameController::getInstance()->_viewlistener->showErrorMsg(g_msgError);
    });
}

void GameController::hideWarningThread()
{
    auto scheduler = cocos2d::Director::getInstance()->getScheduler();
    scheduler->performFunctionInCocosThread([](){
        
        if (GameController::getInstance()->_viewlistener == NULL) {
            
            return;
        }
        //GameController::getInstance()->_viewlistener->hideWarningMsg();
    });
}

/***
 * AppWarp Helper Methods
 */

void GameController::onConnectDone(int res, std::string msg)
{
    auto scheduler = cocos2d::Director::getInstance()->getScheduler();
    
    char strMsg[128] = "";
    if (res==AppWarp::ResultCode::success)
    {
        unscheduleRecover();
        CCLOG("\nonConnectDone .. SUCCESS..session=%d\n",AppWarp::AppWarpSessionID);
        
        //warpClientRef = AppWarp::Client::getInstance();
        //warpClientRef->joinRoom(ROOM_ID);
        
        joinToGameRoom();
    }
    else if (res==AppWarp::ResultCode::success_recovered)
    {
        AppWarp::Arguments param;
        param<<g_mainUserInfo.user_name.c_str();
        
        //warpClientRef->invokeRoomRPC(g_roomId, "requestAllInfo", &param);
        
        unscheduleRecover();
        
        CCLOG("\nonConnectDone .. SUCCESS with success_recovered..session=%d\n",AppWarp::AppWarpSessionID);
    }
    else if (res==AppWarp::ResultCode::connection_error_recoverable)
    {
        isPause = true;
        //
        g_msgWarning = "Connecting ...";
        showWarningMsg();
        
        warpClientRef->recoverConnection();
        
        scheduleRecover();
        
        CCLOG("\nonConnectDone .. FAILED..connection_error_recoverable..session=%d\n",AppWarp::AppWarpSessionID);
    }
    else if (res==AppWarp::ResultCode::bad_request)
    {
        g_msgError = "Bad request ...";//L_CONNECTION_ERROR; //"Bad request ...";
        showErrorMsg();

        unscheduleRecover();
        CCLOG("\nonConnectDone .. FAILED with bad request..session=%d\n",AppWarp::AppWarpSessionID);
        
        //exitYesFunc();
    }
    else if(res==AppWarp::ResultCode::auth_error)
    {
        char strMsg[64] = "";//"Connection failed";
        sprintf(strMsg, "Auth Error : %s", msg.c_str());
        //sprintf(strMsg, "Auth Failed !");
        
        g_msgError = strMsg; //L_CONNECTION_ERROR;//strMsg;
        showErrorMsg();
        
        unscheduleRecover();
        CCLOG("\nonConnectDone .. FAILED with Auth Error..session=%d\n",AppWarp::AppWarpSessionID);
    }
    else
    {
        char strMsg[64] = "";
        sprintf(strMsg, "Connection Error %d, %s", res, msg.c_str());
        
        if (res == 5)
        {
            if (g_nLangType == 0)
            {
                sprintf(strMsg, "Please verify your internet connection");
            }
            else if (g_nLangType == 1)
            {
                sprintf(strMsg, "Por favor verifique su conexión a internet");
            }
        }
        
        //sprintf(strMsg, "Connection failed");
        g_msgError = strMsg; //L_CONNECTION_ERROR;
        
        showErrorMsg();
        
        unscheduleRecover();
        CCLOG("\nonConnectDone .. FAILED with reasonCode=%d, session=%d\n",res, AppWarp::AppWarpSessionID);
    }
}

void GameController::onDisconnectDone(int res)
{
    int nn = res;
    CCLOG("\nonDisConnectDone .. FAILED with session=%d\n",AppWarp::AppWarpSessionID);
    
    warpClientRef->setZoneRequestListener(NULL);
    warpClientRef->setNotificationListener(NULL);
    warpClientRef->setConnectionRequestListener(NULL);
}

void GameController::onGetMatchedRoomsDone(AppWarp::matchedroom event)
{
    int res = event.result;
    int num = event.roomData.size();
}

void GameController::onJoinRoomDone(AppWarp::room event)
{
    int res = event.result;
    if (res == AppWarp::ResultCode::success) { //if room join success
    
        CCLOG("onJoinRoomDone Success \n");
        
        warpClientRef->subscribeRoom(event.roomId);
    }
    else { //if room join fail
        
        if(g_nRoomType == FREE_ROOM) { 
         
            time_t     now = time(0);
            struct tm  tstruct;
            char       buf[80];
            tstruct = *localtime(&now);
            strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
            std::string room_name = buf;
            
            std::string strType = "0";
            std::string strBuyin = StringUtils::format("%.0f", g_nBuyin);
            std::string strBuyinkey = StringUtils::format("%.0f", tmp_roomBuyin_key);
            std::string strFee = StringUtils::format("%d", g_nPlayFee);
            
            std::map<std::string, std::string> table;
            table["type"] = strType;
            table["buyin"] = strBuyinkey;
            table["fee"] = strFee;
            // table["start"] = strBuyin;
            
            
            std::string room_owner = "tian";
            warpClientRef->createTurnRoom(room_name, room_owner, MAX_PLAYERS, table, 120);
        }
        else if(g_nRoomType == MASTER_ROOM) { 
            
            time_t     now = time(0);
            struct tm  tstruct;
            char       buf[80];
            tstruct = *localtime(&now);
            strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
            std::string room_name = buf;
            
            std::string strType = "1";
            std::string strBuyin = StringUtils::format("%d", g_nBuyin);
            std::string strFee = StringUtils::format("%d", g_nPlayFee);
            
            std::map<std::string, std::string> table;
            table["type"] = strType;
            table["buyin"] = strBuyin;
            table["fee"] = strFee;
            
            std::string room_owner = "tian";
            warpClientRef->createTurnRoom(room_name, room_owner, MAX_PLAYERS, table, 120);
        }
        else {
            
            CCLOG("room id = %s, result = %d\n", event.roomId.c_str(), event.result);
            g_msgError = "Room join failed !!!";
            showErrorMsg();
        }
    }
}

void GameController::joinRoomByID(std::string room_id)
{
    warpClientRef->joinRoom(g_roomId);
}

void GameController::onCreateRoomDone(AppWarp::room event)
{
    int res = event.result;
    
    int max_user = event.maxUsers;
    std::string room_name = event.name;
    std::string room_owner = event.owner;
    if (res == AppWarp::ResultCode::success) {
        
        CCLOG("onCreateRoomDone Success \n");
        CCLOG("room id = %s, owner = %s\n", event.roomId.c_str(), room_owner.c_str());
        
        isRoomCreated = true;
        
        g_roomId = event.roomId;
        cocos2d::log("===============================================\nRoomID = %s\n======================================================", g_roomId.c_str());
        warpClientRef->joinRoom(g_roomId);
    }
    else {
        
        CCLOG("onCreateRoomDone : Failed !!!");
        
        g_msgError = "Room creation failed !!!";
        showErrorMsg();
    }
}

void GameController::onGetLiveRoomInfoDone(AppWarp::liveroom event)
{
    int res = event.result;
    if(res == AppWarp::ResultCode::success) {
        
        CCLOG("onGetLiveRoomInfoDone Success \n");
        
        //std::map<std::string, std::string> table = event.properties;
        //std::string strStake = table["num"];
    }
    else {
        
        CCLOG("onGetLiveRoomInfoDone : Failed !!!");
    }
}

void GameController::onSubscribeRoomDone(AppWarp::room event)
{
    int res = event.result;
    if (res == AppWarp::ResultCode::success) {
        
        CCLOG("onSubscribeRoomDone Success \n");
        
        g_roomId = event.roomId;
        //warpClientRef->startGame();
        
        if (g_nRoomType != MASTER_ROOM) {
            
            AppWarp::Arguments param;
            param<<g_mainUserInfo.user_id.c_str();
            warpClientRef->invokeRoomRPC(g_roomId, "requestTableInfo", &param);
        }
        else {
            
            if (_viewlistener != NULL) {
                
                _viewlistener->onMasterRoomCreated();
            }
        }
    }
    else {
        
        CCLOG("onSubscribeRoomDone : Failed !!!");
        
        g_msgError = "Room subscribe failed !!!";
        showErrorMsg();
    }
}

void GameController::notifyRPCMsg()
{
    GameController* pController = GameController::getInstance();
    
    auto t = std::thread(CC_CALLBACK_0(GameController::notifyRPCThread, pController));
    t.detach();
}

void GameController::notifyRPCThread()
{
    auto scheduler = cocos2d::Director::getInstance()->getScheduler();
    scheduler->performFunctionInCocosThread([](){
        
        if (GameController::getInstance()->_viewlistener == NULL) {
            
            return;
        }
        GameController::getInstance()->_viewlistener->onRPCMessage(GameController::getInstance()->msgRPC, GameController::getInstance()->typeRPC);
    });
}

void GameController::onSendRPCDone(AppWarp::RPCResult event)
{
    GameController* pController = GameController::getInstance();
    
    if (event.result == AppWarp::ResultCode::success) {
        
        msgRPC = event.value;
        if (event.func == "requestTableInfo") {
            
            typeRPC = 0;
            CCLOG("receive TableUserInfo RPC message\n");
            
            notifyRPCMsg();
        }
        else if (event.func == "notifyGameCashInfo") {
            
            typeRPC = 1;
            CCLOG("receive notifySettingInfo RPC message\n");
        }
    }
}

void GameController::onChatReceived(AppWarp::chat chatevent)
{
    if (chatevent.sender != APPWARP_SERVERNAME) return;
  
    if (isPause == true) return;
        
    std::string newMsg = chatevent.chat;
    
    //check if user chat message
    std::size_t loc = newMsg.find('#');
    std::string str_code = newMsg.substr(0, loc);
    int val = std::atoi(str_code.c_str());
    
    if (val == USER_MESSAGE) {
        
        CCLOG("onChattingReceived : %s\n", newMsg.c_str());
        std::string str_msg = newMsg.substr(loc+1);
        if (_viewlistener) {
            
            _viewlistener->onUserMessage(str_msg);
        }
        
        return;
    }
    else if(val == USER_EMOTICON) {
        
        CCLOG("onEmoticonReceived : %s\n", newMsg.c_str());
        std::string str_msg = newMsg.substr(loc+1);
        if (_viewlistener) {
            
            _viewlistener->onEmoticonMessage(str_msg);
        }
        
        return;
    }
    else if (val == USER_ITEM) {
        
        CCLOG("onGiftReceived : %s\n", newMsg.c_str());
        std::string str_msg = newMsg.substr(loc+1);
        if (_viewlistener) {
            
            _viewlistener->onGiftMessage(str_msg);
        }
        
        return;
    }
    else if (val == USER_UPDATEINFO) {
        
        CCLOG("onUpdatePhoto : %s\n", newMsg.c_str());
        std::string str_msg = newMsg.substr(loc+1);
        if (_viewlistener) {
            
            _viewlistener->onUpdateInfoMessage(str_msg);
        }
        
        return;
    }
    
    g_responseQueueMutex.lock();
    
    g_responseQueue.push(newMsg);
    
    g_responseQueueMutex.unlock();
    
    // Notify thread start to work thread
    //g_SleepCondition.notify_one();
}

void GameController::endProcessing()
{
    isProcessing = false;
    
    // Notify thread start to work thread
    //g_SleepCondition.notify_one();
}

void GameController::requestGameData()
{
    //warpClientRef->sendChat("get");
    AppWarp::Arguments param;
    param<<g_mainUserInfo.user_id.c_str();
    
    warpClientRef->invokeRoomRPC(g_roomId, "requestTableInfo", &param);
}

void GameController::sendUserInfo()
{
    std::string strCash = StringUtils::format("%.0f", g_mainUserInfo.points);
    
    AppWarp::Arguments param;
    param<<g_mainUserInfo.user_id.c_str();
    param<<strCash.c_str();
    param<<g_mainUserInfo.photo_id.c_str();
    
    warpClientRef->invokeRoomRPC(g_roomId, "notifyUserInfo", &param);
}

void GameController::scheduleRecover()
{
    this->schedule(schedule_selector(GameController::recover), 5.0f);
    
    //showReconnectingLayer("Reconnecting ...");
    
    //startTimerThread();
}

void GameController::unscheduleRecover()
{
    isWaiting = false;
    
    unschedule(schedule_selector(GameController::recover));
}

void GameController::recover(float dt)
{
    AppWarp::Client::getInstance()->recoverConnection();
}
