
#ifndef __PointGame_Controller__
#define __PointGame_Controller__

#include <stdio.h>
#include <queue>

#include "appwarp.h"
#include "common.h"

using namespace std;

class GameScene_Online;

class GameController : public cocos2d::CCNode, public AppWarp::ConnectionRequestListener,public AppWarp::RoomRequestListener,public AppWarp::NotificationListener,public AppWarp::ZoneRequestListener,public AppWarp::TurnBasedRoomRequestListener
{

public:
    //online part
    AppWarp::Client *warpClientRef;
    
    GameController();
    ~GameController();
   
    static GameController* getInstance();
    
    void connect2AppWarp();
    void disconnect2AppWarp();
    void joinToGameRoom();
    void leaveGameRoom();
    
    void joinRoomByID(std::string room_id);
    
    void sendUserInfo();
    //void sendTotalPointInfo(float point);
    void sendChatMessage(std::string msg);
    
    void onConnectDone(int res, std::string msg);
    void onDisconnectDone(int res);
    
    //void onExit();
    
    void onGetMatchedRoomsDone(AppWarp::matchedroom event);
    void onJoinRoomDone(AppWarp::room event);
    void onCreateRoomDone(AppWarp::room event);
    void onSubscribeRoomDone(AppWarp::room event);
    
    void onSendRPCDone(AppWarp::RPCResult event);
    void onGetLiveRoomInfoDone(AppWarp::liveroom event);
    
    void onChatReceived(AppWarp::chat chatevent);
//    void onStartGameDone(int res);
//    void onGameStarted(std::string sender, std::string id, std::string nextTurn);
    //void onMoveCompleted(move event);
    //void onUserPaused(std::string user,std::string locId,bool isLobby);
    //void onUserResumed(std::string user,std::string locId,bool isLobby);
    
    void requestGameData();
    
    void scheduleRecover();
    void unscheduleRecover();
    void recover(float dt);
    
    void setViewListner(GameScene_Online *listener);
    void endProcessing();
    void setPauseFlag(bool flag){ isPause = flag; }
    
    clock_t begin_time = 0, cur_time = 0, keep_time = 0;
    
    queue <std::string> income_queue;
    queue <std::string> outcome_queue;
    
    std::string curMsg, sendMsg;
    
    static void startSendThread();
    static void startRespondThread();
    
    void stopRespondThread();
    
    static void showWarningMsg();
    static void hideWarningMsg();
    static void showErrorMsg();
    static void notifyRPCMsg();
  
    GameScene_Online *_viewlistener;
    
    bool isProcessing;
    bool isPause;
    
    bool isWaiting;
    
    bool isRoomCreated;
    
private:
    
    void senderThread();
    void receiverThread();
    void dispatchResponseCallbacks();
    void notifyRPCThread();
    
    void showWarningThread();
    void hideWarningThread();
    void showErrorThread();
    
    static GameController* _instance;
    
    std::string msgRPC;
    int typeRPC;   

};

#endif /* defined(__PointGame_Controller__) */
