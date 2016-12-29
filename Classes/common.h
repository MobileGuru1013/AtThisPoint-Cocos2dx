//
//  common.h
//

#ifndef __PointGame_Common__
#define __PointGame_Common__

#include <stdio.h>
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
//#include "extensions/cocos-ext.h"

#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

#include "network/HttpClient.h"

#include "define.h"

using namespace cocos2d;
using namespace cocos2d::ui;
using namespace cocostudio;
using namespace cocos2d::network;
using namespace cocostudio::timeline;
//using namespace cocos2d::extension;

class HelloWorld;
class SpinLayer;
class StoreLayer;

#define COCOS2D_DEBUG 1

#define GET_STRING(name, index) name##index

//online part
// Message Constants
#define PLAYER_HAND                 1

#define RESULT_USER_JOIN            2

#define RESULT_USER_LEFT            3

#define RESULT_GAME_START           4

#define RESULT_GAME_OVER            5

#define RESULT_RISK                 6

#define RESULT_SHOW                 7

#define RESULT_PLAY                 8

#define RESULT_PLAY_DONE            9

#define RESULT_TRICK_DONE           10

#define RESULT_TRICK_START          11

#define RESULT_POINT_UPDATE         12

//server status
#define SERVER_RESET                0
#define SERVER_RISK                 1
#define SERVER_DEAL                 2
#define SERVER_SHOW                 3
#define SERVER_PLAY                 4
#define SERVER_FINISH               5
#define SERVER_PLAY_DONE            6
#define SERVER_TRICK_DONE           7

#define USER_MESSAGE                15
#define USER_ITEM                   16
#define USER_EMOTICON               17
#define USER_UPDATEINFO             18

#define APPWARP_APP_KEY   "4a24a467-c437-464a-b"
// #define APPWARP_HOSTNAME   "52.39.200.192"
// #define APPWARP_HOSTNAME   "52.38.250.23"
#define APPWARP_HOSTNAME   "35.161.202.99"

// #define APPWARP_HOSTNAME   "192.168.1.207"
#define APPWARP_SERVERNAME "AppWarpS2"

//#define WEBSERVICE_HOST    "52.39.	200.192"
#define WEBSERVICE_HOST    "35.161.202.99"
// #define WEBSERVICE_HOST    "192.168.1.207"
#define FB_APPID    "1796975997198815"

//#define APPWARP_APP_KEY   "4a24a467-c437-464a-b"
//#define APPWARP_HOSTNAME   "192.168.1.15"
//#define APPWARP_SERVERNAME "AppWarpS2"
//
//#define WEBSERVICE_HOST    "192.168.1.15"
//#define FB_APPID    "1796975997198815"

////////////////////////////////////////
#define MASKING_THRESHOLD           0.04

#define MAIN_USER                   0

#define MAX_LEVEL                   12
#define NUM_CARDS					40
#define CARDS_PER_PLAYER			1
#define MAX_PLAYERS					12
#define MAX_AVATARS                 12
#define MAX_NAMELENGTH              13

#define TURN_TIME                   12

#define ENABLE_OPACITY              255
#define DISABLE_OPACITY             100

//#define MAX(a,b)  (((a) > (b)) ? (a) : (b))
//#define MIN(a,b)  (((a) < (b)) ? (a) : (b))

#define HAS_BIT(x, b) ((x) & (b))
#define SET_BIT(x, b) ((x) |= (b))
#define REMOVE_BIT(x, b) ((x) &= (~b))

typedef enum SpecialType {
	ST_NONE = 0,
	ST_CAIDA = 1,
	ST_LIMPIA = (1 << 1),
	
	ST_CAIDA_LIMPIA = ST_CAIDA | ST_LIMPIA
} SPECIAL_TYPE;

enum CARD_STATUS
{
	INVALID = -1,
	VALID = 0
};

enum GAME_STATUS
{
	STOP_STATUS = -1,
	BEGIN_STATUS,
    RISK_STATUS,
	DEAL_STATUS,
    PLAY_STATUS,
    CHECK_STATUS,
    FINISH_STATUS,
};

enum CARD_SUIT
{
	NONE_SUIT = -1,
	SPADE_SUIT = 0,
	HEART_SUIT = 1,
	CLUB_SUIT = 2,
	DIAMOND_SUIT = 3,

	MAX_SUIT
};

enum SUN_CARDNUMBER
{
	CARD_A = 0,
	CARD_2,
	CARD_3,
	CARD_4,
	CARD_5,
	CARD_6,
	CARD_7,
	CARD_J,
	CARD_Q,
	CARD_K,
	
	MAX_NUMBER
};

enum PLAY_TYPE
{
	FOLD_PLAY = 0,
	BETTING_PLAY,
};

enum LAYER_TYPE
{
    ALERT_LAYER,
    REGISTER_LAYER,
	PROFILE_LAYER,
	AVATAR_LAYER,
	SETTING_LAYER,
    INVITEBONUS_LAYER,
    FBCONNECT_LAYER,
    FRIEND_LAYER,
    SPIN_LAYER,
    TUTORIAL_LAYER,
    STORE_LAYER,
    CUSTOMTABLE_LAYER
};

enum TIME_TYPE
{
    SECOND_TYPE,
    MINUTE_TYPE,
    HOUR_TYPE
};

enum ROOM_TYPE
{
    AI_ROOM,
    FREE_ROOM,
    MASTER_ROOM,
    VIP_ROOM,
};

typedef struct User_Info
{
    std::string user_id;
    std::string fb_id;
    std::string user_name;
    std::string photo_id;
    
    float points;
    int total_play;
    int win_play;
    int level;
    int level_point;
    
    float max_earning;
    int win_streak;
    int total_hour;
    int total_cleanpot;
    float total_earning;
    int total_tourplay;
    int total_emoticon;
    int total_invite;
    int total_avatars;
    
    int signup_flag;
    int push_flag;
    int friend_flag;
    
    int last_gift;
    int last_msg;
    
    int total_toprank;
    int rank;
    float rank_point;
    int gift_item;
    
    User_Info() {
        
        user_id = "";
        fb_id = "";
        user_name = "Guest";
        photo_id = "0";
        
        points = 0;
        total_play = 0;
        win_play = 0;
        level = 0;
        level_point = 0;
        
        max_earning = 0;
        win_streak = 0;
        total_hour = 0;
        total_cleanpot = 0;
        total_earning = 0;
        total_tourplay = 0;
        total_emoticon = 0;
        total_invite = 0;
        total_avatars = 0;
        
        total_toprank = 0;
        rank = 0;
        rank_point = 0;
        
        signup_flag = 0;
        push_flag = 0;
        friend_flag = 0;
        gift_item = -1;
    }
}USER_INFO;

typedef struct Friend_Info
{
    std::string user_id;
    std::string fb_id;
    std::string user_name;
    std::string photo_id;
    
    bool flag;
    
    int level;
    float points;

    float total_play;
    int win_play;
    float max_earning;
    int win_streak;
    
    Friend_Info() {
        
        user_id = "";
        fb_id = "";
        user_name = "";
        photo_id = "";
        
        level = 0;
        points = 0;
        total_play = 0;
        win_play = 0;
        max_earning = 0;
        win_streak = 0;
        
        flag = false;
    }
}FRIEND_INFO;

typedef struct Rank_Info
{
    std::string user_id;
    std::string user_name;
    std::string photo_id;
    
    int level;
    float total_earning;
    
    Rank_Info() {
        
        user_id = "";
        user_name = "";
        photo_id = "";
        
        level = 0;
        total_earning = 0;
    }
}RANK_INFO;

typedef struct Gift_Info
{
    int index;
    int type;
    float bonus;
    
    Gift_Info() {
        
        index = INVALID;
        type = INVALID;
        bonus = 0;
    }
}GIFT_INFO;

typedef struct Msg_Info
{
    int index;
    std::string sender_id;
    std::string name;
    std::string photo_id;

    std::string title;
    std::string msg;
    
    int type;
    time_t sent_time;
}MSG_INFO;

extern int g_nLimitLoadingCount;
extern int g_nFreeSpin;
extern time_t g_timeLastFreeSpin;

extern HelloWorld *g_pHelloWorld;
extern SpinLayer *g_pSpinLayer;
extern StoreLayer *g_pStoreLayer;

extern std::vector<FRIEND_INFO> g_vFriends;
extern std::vector<FRIEND_INFO> g_vFBFriends;
extern std::vector<USER_INFO> g_vSearchUsers;
extern std::vector<GIFT_INFO> g_vGifts;
extern std::vector<MSG_INFO> g_vMessages;

extern std::vector<RANK_INFO>g_vFriendsRanking;
extern std::vector<RANK_INFO>g_vGlobalRanking;

extern USER_INFO g_mainUserInfo;
extern std::string g_roomId;
extern int g_nRoomType;
extern int g_nTablePos;
extern int g_nPlayFee;
extern float g_nBuyin;

extern float g_fScaleWH;

extern std::string g_msgWarning;
extern std::string g_msgError;

extern bool g_bLoginFB;

extern bool g_bChatBan[MAX_PLAYERS];
extern int g_nAvatarPoint[MAX_AVATARS];
extern int g_nFeeList[4];
extern int g_nBuyinList[4];

extern bool g_bSound;
extern bool g_bMusic;
extern bool g_bQplay;
extern int g_nLangType;

extern std::string g_strDeviceToken;
extern std::string g_strDeviceID;
extern time_t g_startTime2Bonus;

extern int g_nLevelupXP[MAX_LEVEL];

extern float g_fScaleX;
extern float g_fScaleY;

int getScreenWidth();
void setScreenWidth(int width);
int getScreenHeight();
void setScreenHeight(int height);

void initProfileData();
bool readProfileData();
bool saveProfileData();

bool addFriend(FRIEND_INFO info);
bool addMsg(MSG_INFO info);
bool addGift(GIFT_INFO info);

void addItem2ScrollView(Layout* item, int id, int cntViewCol, int cntTotalRow, ScrollView *sv_list);
void setImageFromUrl(std::string url, ImageView *img_photo);

void setSelectAvatar(int index);
int getSelectAvatar();
void setAvatarUnlock(int index, int value);
int getAvatarUnlock(int index);

void setGiftBonusTime(time_t cur_time);
time_t getGiftBonusTime();
void setGiftBonusFlag(bool flag);
bool getGiftBonusFlag();

void setLessGiftFlag(bool flag);
bool getLessGiftFlag();

void setShowedAds(bool flag);
bool getShowedAds();


std::string getNormalText2Number(float value);
std::vector<std::string> getNormalMessages(std::string msg);
void getUppercaseString(std::string &str);
std::string getProperName(const char *strOriginName, int limitLength);

void checkTotalPoint();

int getDurationTime(time_t st_time, int type);

extern std::string tmp_fb_username;
extern bool tmp_prev_isFB;
extern float tmp_roomBuyin_key;
extern bool tmp_isShowedAds;
extern bool less_gift_flag;
#endif /* defined(__PointGame_Common__) */
