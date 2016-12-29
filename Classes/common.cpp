//
//  common.cpp
//

#include "common.h"
#include "HelloWorldScene.h"
#include "GameScene_Online.h"
#include "SpinLayer.h"
#include "StoreLayer.h"
#include "WebService.h"
#include "AppService.h"

int g_nLevelupXP[MAX_LEVEL] = {794, 2382, 4764, 7940, 11910, 16674, 22232, 28584, 34936, 42876, 50816,};
int g_nAvatarPoint[MAX_AVATARS] = {10000, 10000, 10000, 15000, 15000, 15000, 30000, 30000, 30000, 40000, 40000, 40000};

int g_nFeeList[4] = {20, 100, 200, 1000};
// int g_nFeeList[4] = {1000, 3000, 6000, 10000};
int g_nBuyinList[4] = {1000, 3000, 6000, -1};

int g_nLimitLoadingCount = 0;
int g_nFreeSpin = 0;
time_t g_timeLastFreeSpin;

HelloWorld *g_pHelloWorld = NULL;
SpinLayer *g_pSpinLayer = NULL;
StoreLayer *g_pStoreLayer = NULL;

std::string g_msgWarning;
std::string g_msgError;

//friend list
std::vector<FRIEND_INFO> g_vFriends;
std::vector<FRIEND_INFO> g_vFBFriends;

std::vector<USER_INFO> g_vSearchUsers;
std::vector<GIFT_INFO> g_vGifts;
std::vector<MSG_INFO> g_vMessages;

std::vector<RANK_INFO>g_vFriendsRanking;
std::vector<RANK_INFO>g_vGlobalRanking;

//profile info
USER_INFO g_mainUserInfo;
std::string g_roomId;
int g_nRoomType;
int g_nTablePos = INVALID;
int g_nPlayFee;
float g_nBuyin;

float g_fScaleWH;

//setting info
bool g_bSound = true;
bool g_bMusic = true;
bool g_bQplay = false;
int g_nLangType = 0;

std::string g_strDeviceToken = "";
std::string g_strDeviceID = "";
time_t g_startTime2Bonus;

bool g_bLoginFB = false;

bool g_bChatBan[MAX_PLAYERS];

int g_screen_width;
int g_screenHeight;

float g_fScaleX, g_fScaleY;

int getScreenWidth()
{
	return g_screen_width;
}
void setScreenWidth(int width)
{
	g_screen_width = width;
}

int getScreenHeight()
{
	return g_screenHeight;
}
void setScreenHeight(int height)
{
	g_screenHeight = height;
}

bool addFriend(FRIEND_INFO info)
{
    //check if there is friend
    for (int i = 0; i < g_vFriends.size(); i++) {
        
        FRIEND_INFO tmp_info = g_vFriends[i];
        if(tmp_info.user_id.compare(info.user_id) == 0) {
            
            return false;
        }
    }
    
    g_vFriends.push_back(info);
    
    return true;
}

bool addMsg(MSG_INFO info)
{
    //check if there is message
    for (int i = 0; i < g_vMessages.size(); i++) {
        
        MSG_INFO tmp_info = g_vMessages[i];
        if(tmp_info.index == info.index) {
            
            return false;
        }
    }
    
    g_vMessages.push_back(info);
    
    return true;
}

bool addGift(GIFT_INFO info)
{
    //check if there is gift
    for (int i = 0; i < g_vGifts.size(); i++) {
        
        GIFT_INFO tmp_info = g_vGifts[i];
        if(tmp_info.index == info.index) {
            
            return false;
        }
    }
    
//    if (info.index > g_mainUserInfo.last_gift) {
//        
//        //g_mainUserInfo.last_gift = info.index;
//        if (info.type == 0) {
//            
//            g_mainUserInfo.points += info.bonus;            
//            WebService::getInstance()->updatePoint();
//        }
//    }
    
    g_vGifts.push_back(info);
    
    return true;
}

void initProfileData()
{
    srand(time(NULL));
    //generate random id
    int digits[5] = {0,};
    for (int i = 0; i < 5; i++) {
        
        digits[i] = rand()%10;
    }
    g_mainUserInfo.user_id = StringUtils::format("%d%d%d%d%d", digits[0], digits[1], digits[2], digits[3], digits[4]);
    g_mainUserInfo.user_name = "Guest" + g_mainUserInfo.user_id;
    g_mainUserInfo.photo_id = "";
    
    g_mainUserInfo.fb_id = "";//"1234567890";
    g_mainUserInfo.points = 300;
    g_mainUserInfo.total_play = 0;
    g_mainUserInfo.win_play = 0;
    g_mainUserInfo.level = 0;
    g_mainUserInfo.level_point = 0;
}

bool readProfileData()
{
    int value;
    std::string configFile = FileUtils::getInstance()->getWritablePath() + "profile.ini";
    FILE* fp = fopen(configFile.c_str(), "rb");
    if (fp != NULL) {
        
        char strTmp[64] = "";
        
        //user id
        fread(strTmp, sizeof(char), 64, fp);
        g_mainUserInfo.user_id = StringUtils::format("%s", strTmp);
        
        //fb id
        fread(strTmp, sizeof(char), 64, fp);
        g_mainUserInfo.fb_id = StringUtils::format("%s", strTmp);
        
        //user name
        fread(strTmp, sizeof(char), 64, fp);
        g_mainUserInfo.user_name = StringUtils::format("%s", strTmp);
        
        //photo id
        fread(strTmp, sizeof(char), 64, fp);
        g_mainUserInfo.photo_id = StringUtils::format("%s", strTmp);
        
        //diamonds
        fread(&value, sizeof(float), 1, fp);
        g_mainUserInfo.points = value;
        
        //total play
        fread(&value, sizeof(int), 1, fp);
        g_mainUserInfo.total_play = value;
        
        //win play
        fread(&value, sizeof(int), 1, fp);
        g_mainUserInfo.win_play = value;

        //caida play
        fread(&value, sizeof(int), 1, fp);
        g_mainUserInfo.level = value;

        //limpia play
        fread(&value, sizeof(int), 1, fp);
        g_mainUserInfo.level_point = value;
        
        fclose(fp);
        
        return true;
    }
    
    return false;
}

bool saveProfileData()
{
    int value;
    std::string configFile = FileUtils::getInstance()->getWritablePath() + "profile.ini";
    FILE* fp = fopen(configFile.c_str(), "w+b");
    if (fp != NULL) {
        
        char strTmp[64] = "";
        
        //user id
        sprintf(strTmp, "%s", g_mainUserInfo.user_id.c_str());
        fwrite(strTmp, sizeof(char), 64, fp);
        
        //fb id
        sprintf(strTmp, "%s", g_mainUserInfo.fb_id.c_str());
        fwrite(strTmp, sizeof(char), 64, fp);
        
        //user name
        sprintf(strTmp, "%s", g_mainUserInfo.user_name.c_str());
        fwrite(strTmp, sizeof(char), 64, fp);
        
        //photo id
        sprintf(strTmp, "%s", g_mainUserInfo.photo_id.c_str());
        fwrite(strTmp, sizeof(char), 64, fp);
        
        //diamonds
        value = g_mainUserInfo.points;
        fwrite(&value, sizeof(float), 1, fp);
        
        //total play
        value = g_mainUserInfo.total_play;
        fwrite(&value, sizeof(int), 1, fp);
        
        //win play
        value = g_mainUserInfo.win_play;
        fwrite(&value, sizeof(int), 1, fp);
        
        //caida play
        value = g_mainUserInfo.level;
        fwrite(&value, sizeof(int), 1, fp);
        
        //limpia play
        value = g_mainUserInfo.level_point;
        fwrite(&value, sizeof(int), 1, fp);
        
        fclose(fp);
        
        return true;
    }
    
    return false;
}

void addItem2ScrollView(Layout* item, int id, int cntViewCol, int cntTotalRow, ScrollView *sv_list)
{
    int row = id / cntViewCol;
    int col = id % cntViewCol;
    
    int wItemView = item->getBoundingBox().size.width;
    int hItemView = item->getBoundingBox().size.height;
    
    Vec2 posItem;
    posItem.x = (col)*wItemView;
    posItem.y = (cntTotalRow-row-1)*hItemView;
    item->setPosition(posItem);
    cocos2d::log("%f, %f", posItem.x, posItem.y);
    sv_list->addChild(item);
}

void setImageFromUrl(std::string url, ImageView *img_photo)
{
    //find image from texture cache, first
    auto pSpriteFrame = cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(url);
    if (pSpriteFrame != nullptr) {
        
        img_photo->loadTexture(url, cocos2d::ui::TextureResType::PLIST);
        return;
    }
    
    auto request = new HttpRequest();
    request->setUrl(url.c_str());
    request->setRequestType(cocos2d::network::HttpRequest::Type::GET);
    
    request->setResponseCallback([img_photo, url](cocos2d::network::HttpClient *, cocos2d::network::HttpResponse *response) {
        
        if (!response->isSucceed()) {return;}
        
        auto responseData = response->getResponseData();
        auto bytes = reinterpret_cast<unsigned char *>(&(responseData->front()));
        
        auto data = cocos2d::Data();
        data.copy(bytes, responseData->size());
        
        auto image = new (std::nothrow) cocos2d::Image();
        if (!image) {return;}
        
        image->initWithImageData(data.getBytes(), data.getSize());
        auto texture = cocos2d::Director::getInstance()->getTextureCache()->addImage(image, url);
        
        auto spriteFrame = cocos2d::SpriteFrame::createWithTexture(texture, cocos2d::Rect(0, 0, texture->getContentSize().width, texture->getContentSize().height));
        cocos2d::SpriteFrameCache::getInstance()->addSpriteFrame(spriteFrame, url);
        CC_SAFE_RELEASE(image);
        
        Director::getInstance()->getScheduler()->performFunctionInCocosThread([img_photo, url](){
        
            img_photo->loadTexture(url, cocos2d::ui::TextureResType::PLIST);
        
        });
    });
    HttpClient::getInstance()->send(request);
    request->release();
    
    //    char url[1024] = "";
    //    //sprintf(url, "https://graph.facebook.com/%s/picture?type=small", user_id.c_str());
    //    sprintf(url, "https://graph.facebook.com/%s/picture?height=128&width=128", user_id.c_str());
    //
    //    HttpRequest* request = new HttpRequest();
    //    request->setRequestType(HttpRequest::Type::GET);
    //    request->setUrl(url);
    //    request->setResponseCallback(CC_CALLBACK_2(HelloWorld::onImgDownloadCompleted, this));
    //    request->setTag("Get Invitable Friend Image");
    //    //HttpClient::getInstance()->send(request);
    //    HttpClient::getInstance()->sendImmediate(request);
    //
    //    request->release();
    //
    //    //WebService::getInstance()->login();
}

std::string getNormalText2Number(float value)
{
    std::string numWithCommas = StringUtils::format("%.0f", value);
    int insertPosition = numWithCommas.length() - 3;
    while (insertPosition > 0) {
        numWithCommas.insert(insertPosition, ",");
        insertPosition-=3;
    }
    
    return numWithCommas;
}

bool BothAreSpaces(char lhs, char rhs)
{
    return (lhs == rhs) && (lhs == ' ');
}

std::vector<std::string> getNormalMessages(std::string msg)
{
    std::vector<std::string> vMessages;
    
    int limit_length = 16;
    std::string tmpMsg = msg;
    
    //change sequence spaces to one space
    std::string::iterator new_end = std::unique(tmpMsg.begin(), tmpMsg.end(), BothAreSpaces);
    tmpMsg.erase(new_end, tmpMsg.end());
   
    int len = tmpMsg.length();
    while (len > limit_length) {
        
        std::string newMsg;
        size_t loc = tmpMsg.find_last_of(' ', limit_length);
        if(loc == std::string::npos) { //if don't find space
            
            loc = limit_length;
        }

        newMsg = tmpMsg.substr(0, loc);
        tmpMsg = tmpMsg.substr(loc+1);
        
        vMessages.push_back(newMsg);
        
        len = tmpMsg.length();
    }
    
    vMessages.push_back(tmpMsg);
    
    return vMessages;
}

void getUppercaseString(std::string &str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

int getDurationTime(time_t st_time, int type)
{
    time_t cur_time;
    time(&cur_time);
    long duration = (long)difftime(cur_time, st_time);
    
    int hour = duration / 60 / 60;
    int minute = (duration / 60) % 60;
    int second = duration % 60;
    
    if (type == SECOND_TYPE) { //second expression
        
        return second;
    }
    else if(type == MINUTE_TYPE) { //minute expression
        
        return minute;
    }
    else if(type == HOUR_TYPE) { //hour expression
        
        return hour;
    }
}

void setSelectAvatar(int index)
{
    g_mainUserInfo.total_avatars &= 0xffff0000;
    SET_BIT(g_mainUserInfo.total_avatars, index);
}

int getSelectAvatar()
{
    int index = HAS_BIT(g_mainUserInfo.total_avatars, 0xffff);
    
    return index;
}

void setAvatarUnlock(int index, int value)
{
    int mask = ~(1<<(16+index));
    g_mainUserInfo.total_avatars = (g_mainUserInfo.total_avatars & mask) | (value<<(16+index));
    //SET_BIT(g_mainUserInfo.total_avatars, value);
}

int getAvatarUnlock(int index)
{
    int value = HAS_BIT(g_mainUserInfo.total_avatars, 1<<(16+index));
    
    return value;
}

void setGiftBonusTime(time_t cur_time)
{
    cocos2d::UserDefault::getInstance()->setIntegerForKey(TIME_KEY, cur_time);
}

time_t getGiftBonusTime()
{
    return cocos2d::UserDefault::getInstance()->getIntegerForKey(TIME_KEY);
}

void setGiftBonusFlag(bool flag)
{
    setGiftBonusTime(0);
    cocos2d::UserDefault::getInstance()->setBoolForKey(BONUS_KEY, flag);
    
    if (flag == true) {
        
        AppService::startBonusNotification();
    } else {
        
        AppService::cancelBonusNotification();
    }
}

bool getGiftBonusFlag()
{
    cocos2d::UserDefault::getInstance()->getBoolForKey(BONUS_KEY);
}

void checkTotalPoint()
{
    time_t bonusTime = getGiftBonusTime();
    if (bonusTime != 0) return;
    
    if (g_mainUserInfo.points < 20) {
        
        time_t cur_time;
        time(&cur_time);
        setGiftBonusTime(cur_time);
    }
    
    return;
}

std::string getProperName(const char *strOriginName, int limitLength)
{
    std::string strNewName;
    
    std::string strTmp = strOriginName;
    char *pos = strchr(strTmp.c_str(), '\n');
    if (pos != nullptr) {
        
        int len = (int)(pos-(strTmp.c_str()));
        strOriginName = strTmp.substr(0, len).c_str();
    }
    
    int len = strlen(strOriginName);
    if (len > limitLength) {
        
        char strTmp[256] = "";
        strncpy(strTmp, strOriginName, limitLength-1);
        strNewName = StringUtils::format("%s..", strTmp);
    }
    else {
        
        strNewName = strOriginName;
    }
    
    return strNewName;
}


std::string tmp_fb_username;
bool tmp_prev_isFB;
float tmp_roomBuyin_key = -1;
bool tmp_isShowedAds = false;
bool less_gift_flag = false;

void setLessGiftFlag(bool flag){
    cocos2d::UserDefault::getInstance()->setBoolForKey("LessGift", flag);
}
bool getLessGiftFlag(){
    cocos2d::UserDefault::getInstance()->getBoolForKey("LessGift");
}

void setShowedAds(bool flag){
    cocos2d::UserDefault::getInstance()->setBoolForKey("showedAds", flag);
}
bool getShowedAds(){
    cocos2d::UserDefault::getInstance()->getBoolForKey("showedAds");
}