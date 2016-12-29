//
//  WebService.cpp
//  Point_Online
//
//  Created by TianXinxing on 5/19/16.
//
//

#include "WebService.h"
#include "cJSON.h"

#include "AppService.h"
#include "HelloWorldScene.h"
#include "Facebook.h"

WebService* WebService::_instance = NULL;

USING_NS_CC;

WebService* WebService::getInstance()
{
    if (_instance == NULL) {
        
        _instance = new WebService();
    }
    
    return _instance;
}

WebService::WebService()
{
    
}

WebService::~WebService()
{
    
}

void WebService::login2Guest()
{
    std::string strUrl = StringUtils::format("http://%s:8080/sys/member/mobile/memberLogin.do", WEBSERVICE_HOST);
    
    HttpRequest* request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::POST);
    request->setUrl(strUrl.c_str());
    request->setResponseCallback(CC_CALLBACK_2(WebService::onGuestLoginCompleted, this));
    
    std::string strDevID = AppService::getDeviceID();//"36F0D48E-A5C4-40ED-89BB-22DA05364BDF";
    
    int dev_type;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    
    dev_type = 1;
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
    dev_type = 2;
#endif
    
    // write the post data
    std::string strData = StringUtils::format("NAME=%s&PHOTO_ID=%s&DEV_ID=%s&TOKEN=%s&DEV_TYPE=%d&AVATARS=%d",
                                              g_mainUserInfo.user_name.c_str(),
                                              g_mainUserInfo.photo_id.c_str(),
                                              strDevID.c_str(),
                                              g_strDeviceToken.c_str(),
                                              dev_type,
                                              g_mainUserInfo.total_avatars
                                            );
    request->setRequestData(strData.c_str(), strData.length());
    
    request->setTag("Guset Login");
    //HttpClient::getInstance()->send(request);
    HttpClient::getInstance()->sendImmediate(request);
    
    request->release();
}

void WebService::login2FB(std::string fb_id)
{
    std::string strUrl = StringUtils::format("http://%s:8080/sys/member/mobile/memberLogin2.do", WEBSERVICE_HOST);
    
    HttpRequest* request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::POST);
    request->setUrl(strUrl.c_str());
    request->setResponseCallback(CC_CALLBACK_2(WebService::onFBLoginCompleted, this));
    
    std::string strDevID = AppService::getDeviceID();
    
    int dev_type;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    
    dev_type = 1;
#else if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
    dev_type = 2;
#endif
    
    // write the post data
    std::string strData = StringUtils::format("FB_ID=%s&NAME=%s&DEV_ID=%s&TOKEN=%s&DEV_TYPE=%d",
                                              fb_id.c_str(),
                                              tmp_fb_username.c_str(),// g_mainUserInfo.user_name.c_str(),
                                              strDevID.c_str(),
                                              g_strDeviceToken.c_str(),
                                              dev_type
                                              );
    
    cocos2d::log("FB Request Parameter : %s", strData.c_str());
    request->setRequestData(strData.c_str(), strData.length());
    
    request->setTag("FB Login");
    //HttpClient::getInstance()->send(request);
    HttpClient::getInstance()->sendImmediate(request);
    
    request->release();
}

void WebService::onGuestLoginCompleted(HttpClient* sender, HttpResponse* response)
{
    AppService::hideCircleProgressbar();
    if (response->isSucceed() == false || response->getResponseCode() != 200){
        
        return;
    }
    
    std::vector<char>buffer = *response->getResponseData();
    std::string str_msg = &buffer[0];
    
    //parsing data
    cJSON *json = cJSON_Parse(str_msg.c_str());
    if (json == NULL) {
        
        return;
    }
    
    cJSON *jsonRes = cJSON_GetObjectItem(json, "retcode");
    cocos2d::log("return code = %d", jsonRes->valueint );
    if (jsonRes == NULL || jsonRes->valueint != 0) {
        
        return;
    }
    
    g_mainUserInfo.signup_flag = cJSON_GetObjectItem(json, "signup")->valueint;
    
    cJSON* data = cJSON_GetObjectItem(json, "user");
    
    g_mainUserInfo.fb_id = "";
    g_mainUserInfo.user_id = cJSON_GetObjectItem(data, "userid")->valuestring;
    g_mainUserInfo.photo_id = cJSON_GetObjectItem(data, "photo_id")->valuestring;
    g_mainUserInfo.user_name = cJSON_GetObjectItem(data, "name")->valuestring;
    g_mainUserInfo.points = cJSON_GetObjectItem(data, "point")->valuedouble;
    g_mainUserInfo.level = cJSON_GetObjectItem(data, "level")->valueint;
    g_mainUserInfo.level_point = cJSON_GetObjectItem(data, "levelpoint")->valueint;
    g_mainUserInfo.total_play = cJSON_GetObjectItem(data, "totalplay")->valueint;
    g_mainUserInfo.win_play = cJSON_GetObjectItem(data, "winplay")->valueint;
    
    g_mainUserInfo.max_earning = cJSON_GetObjectItem(data, "max_earning")->valueint;
    g_mainUserInfo.win_streak = cJSON_GetObjectItem(data, "win_streak")->valueint;
    g_mainUserInfo.total_hour = cJSON_GetObjectItem(data, "total_hour")->valueint;
    g_mainUserInfo.total_cleanpot = cJSON_GetObjectItem(data, "total_cleanpot")->valueint;
    g_mainUserInfo.total_earning = cJSON_GetObjectItem(data, "total_earning")->valueint;
    g_mainUserInfo.total_tourplay = cJSON_GetObjectItem(data, "total_tournament")->valueint;
    g_mainUserInfo.total_emoticon = cJSON_GetObjectItem(data, "total_emoticon")->valueint;
    g_mainUserInfo.total_invite = cJSON_GetObjectItem(data, "total_invite")->valueint;
    g_mainUserInfo.total_avatars = cJSON_GetObjectItem(data, "total_avatars")->valueint;
    g_mainUserInfo.last_msg = cJSON_GetObjectItem(data, "last_msg")->valueint;
    g_mainUserInfo.last_gift = cJSON_GetObjectItem(data, "last_gift")->valueint;
    
    g_nFreeSpin = cJSON_GetObjectItem(data, "free_spin")->valueint;
    g_timeLastFreeSpin = cJSON_GetObjectItem(data, "last_spintime")->valueint;
    g_mainUserInfo.push_flag = cJSON_GetObjectItem(data, "push_flag")->valueint;

    cocos2d::UserDefault::getInstance()->setStringForKey(ID_KEY, g_mainUserInfo.user_id);
    
    
    
    cJSON_Delete(json);
    
    if(g_bLoginFB == true) {
    
        g_bLoginFB = false;
        Facebook::getInstance()->logout();
        
        if (g_pHelloWorld != NULL) {
            
            g_pHelloWorld->showProfileInfo();
        }
    }
    
    if (g_mainUserInfo.signup_flag == 1) {
        
        time(&g_timeLastFreeSpin);
        updateFreeSpin();
        g_nLimitLoadingCount = 100;
    }
    else {

        g_nLimitLoadingCount = 70;
        getFriendList();
    }
    
}

void WebService::onFBLoginCompleted(HttpClient* sender, HttpResponse* response)
{
    AppService::hideCircleProgressbar();
    
    if (response->isSucceed() == false || response->getResponseCode() != 200){
        
        return;
    }
    
    std::vector<char>buffer = *response->getResponseData();
    std::string str_msg = &buffer[0];
    
    //parsing data
    cJSON *json = cJSON_Parse(str_msg.c_str());
    if (json == NULL) {
        
        return;
    }
    
    cJSON *jsonRes = cJSON_GetObjectItem(json, "retcode");
    if (jsonRes->valueint != 0) {
        
        return;
    }
    
    int isSignup = cJSON_GetObjectItem(json, "signup")->valueint;
    
    cJSON* data = cJSON_GetObjectItem(json, "user");
    
    g_mainUserInfo.user_id = cJSON_GetObjectItem(data, "userid")->valuestring;
    g_mainUserInfo.fb_id = cJSON_GetObjectItem(data, "fb_id")->valuestring;
    g_mainUserInfo.photo_id = cJSON_GetObjectItem(data, "photo_id")->valuestring;
    g_mainUserInfo.user_name = cJSON_GetObjectItem(data, "name")->valuestring;
    g_mainUserInfo.points = cJSON_GetObjectItem(data, "point")->valuedouble;
    g_mainUserInfo.level = cJSON_GetObjectItem(data, "level")->valueint;
    g_mainUserInfo.level_point = cJSON_GetObjectItem(data, "levelpoint")->valueint;
    g_mainUserInfo.total_play = cJSON_GetObjectItem(data, "totalplay")->valueint;
    g_mainUserInfo.win_play = cJSON_GetObjectItem(data, "winplay")->valueint;
    g_mainUserInfo.max_earning = cJSON_GetObjectItem(data, "max_earning")->valueint;
    g_mainUserInfo.win_streak = cJSON_GetObjectItem(data, "win_streak")->valueint;
    g_mainUserInfo.total_hour = cJSON_GetObjectItem(data, "total_hour")->valueint;
    g_mainUserInfo.total_cleanpot = cJSON_GetObjectItem(data, "total_cleanpot")->valueint;
    g_mainUserInfo.total_earning = cJSON_GetObjectItem(data, "total_earning")->valueint;
    g_mainUserInfo.total_tourplay = cJSON_GetObjectItem(data, "total_tournament")->valueint;
    g_mainUserInfo.total_emoticon = cJSON_GetObjectItem(data, "total_emoticon")->valueint;
    g_mainUserInfo.total_invite = cJSON_GetObjectItem(data, "total_invite")->valueint;
    g_mainUserInfo.total_avatars = cJSON_GetObjectItem(data, "total_avatars")->valueint;
    g_mainUserInfo.last_msg = cJSON_GetObjectItem(data, "last_msg")->valueint;
    g_mainUserInfo.last_gift = cJSON_GetObjectItem(data, "last_gift")->valueint;
    
    g_nFreeSpin = cJSON_GetObjectItem(data, "free_spin")->valueint;
    g_timeLastFreeSpin = cJSON_GetObjectItem(data, "last_spintime")->valueint;
    g_mainUserInfo.push_flag = cJSON_GetObjectItem(data, "push_flag")->valueint;
    
    //cocos2d::UserDefault::getInstance()->setStringForKey(ID_KEY, g_mainUserInfo.user_id);
    
    cJSON_Delete(json);
    
    g_bLoginFB = true;
    cocos2d::UserDefault::getInstance()->setStringForKey(FBID_KEY, g_mainUserInfo.fb_id);
    if (g_pHelloWorld != NULL) {
        
        g_pHelloWorld->showProfileInfo();
        if (isSignup == 1) {
            
            time(&g_timeLastFreeSpin);
            updateFreeSpin();
            g_pHelloWorld->showFBBonusLayer();
        }
        else {
            
            g_pHelloWorld->showFBInviteLayer();
        }
    }
    
    //saveProfileData();
    getFriendList();
}

//get user profile info
void WebService::getUserProfile(std::string user_id)
{
    if(user_id.empty() == true) return;
    
    std::string strURL = StringUtils::format("http://%s:8080/sys/member/mobile/getUserInfo.do", WEBSERVICE_HOST);
    
    HttpRequest* request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::POST);
    request->setUrl(strURL.c_str());
    request->setResponseCallback(CC_CALLBACK_2(WebService::onFriendListCompleted, this));
    
    // write the post data
    std::string strData = StringUtils::format("USER_ID=%s",
                                              user_id.c_str()
                                              );
    request->setRequestData(strData.c_str(), strData.length());
    
    request->setTag("get UserInfo");
    //HttpClient::getInstance()->send(request);
    HttpClient::getInstance()->sendImmediate(request);
    
    request->release();
}

void WebService::onUserProfileCompleted(HttpClient* sender, HttpResponse* response)
{
    if (response->isSucceed() == false){
        
        return;
    }
    
    std::vector<char>buffer = *response->getResponseData();
    std::string str_msg = &buffer[0];
    
    //parsing data
    cJSON *json = cJSON_Parse(str_msg.c_str());
    if (json == NULL) {
        
        return;
    }
    
    cJSON *jsonRes = cJSON_GetObjectItem(json, "retcode");
    if (jsonRes->valueint != 0) {
        
        return;
    }
    
    cJSON* data = cJSON_GetObjectItem(json, "user");
    
    USER_INFO userInfo;
    
    userInfo.user_id = cJSON_GetObjectItem(data, "userid")->valuestring;
    userInfo.photo_id = cJSON_GetObjectItem(data, "photo_id")->valuestring;
    userInfo.user_name = cJSON_GetObjectItem(data, "name")->valuestring;
    userInfo.points = cJSON_GetObjectItem(data, "point")->valuedouble;
    userInfo.level = cJSON_GetObjectItem(data, "level")->valueint;
    userInfo.level_point = cJSON_GetObjectItem(data, "levelpoint")->valueint;
    userInfo.total_play = cJSON_GetObjectItem(data, "totalplay")->valueint;
    userInfo.win_play = cJSON_GetObjectItem(data, "winplay")->valueint;
    
    userInfo.max_earning = cJSON_GetObjectItem(data, "max_earning")->valueint;
    userInfo.win_streak = cJSON_GetObjectItem(data, "win_streak")->valueint;
    userInfo.total_hour = cJSON_GetObjectItem(data, "total_hour")->valueint;
    userInfo.total_cleanpot = cJSON_GetObjectItem(data, "total_cleanpot")->valueint;
    userInfo.total_earning = cJSON_GetObjectItem(data, "total_earning")->valueint;
    userInfo.total_tourplay = cJSON_GetObjectItem(data, "total_tournament")->valueint;
    userInfo.total_emoticon = cJSON_GetObjectItem(data, "total_emoticon")->valueint;
    userInfo.total_invite = cJSON_GetObjectItem(data, "total_invite")->valueint;
    userInfo.total_avatars = cJSON_GetObjectItem(data, "total_avatars")->valueint;
    
    cocos2d::UserDefault::getInstance()->setStringForKey(ID_KEY, g_mainUserInfo.user_id);
    
    cJSON_Delete(json);
}

void WebService::getFriendList()
{
    std::string strUrl = StringUtils::format("http://%s:8080/sys/friend/mobile/friendUsers.do", WEBSERVICE_HOST);
    
    HttpRequest* request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::POST);
    request->setUrl(strUrl.c_str());
    request->setResponseCallback(CC_CALLBACK_2(WebService::onFriendListCompleted, this));
    
    // write the post data
    std::string strData = StringUtils::format("USER_ID=%s",
                                              g_mainUserInfo.user_id.c_str()
                                              );
    request->setRequestData(strData.c_str(), strData.length());
    
    request->setTag("get Friend list");
    //HttpClient::getInstance()->send(request);
    HttpClient::getInstance()->sendImmediate(request);
    
    request->release();

}

void WebService::onFriendListCompleted(HttpClient* sender, HttpResponse* response)
{
    if (response->isSucceed() == false){
        
        return;
    }
    
    std::vector<char>buffer = *response->getResponseData();
    std::string str_msg = &buffer[0];
    
    //parsing data
    cJSON *json = cJSON_Parse(str_msg.c_str());
    if (json == NULL) {
        
        return;
    }
    
    cJSON *jsonRes = cJSON_GetObjectItem(json, "retcode");
    if (jsonRes->valueint != 0) {
        
        return;
    }
    
    g_vFriends.clear();
    
    cJSON* datas = cJSON_GetObjectItem(json, "friends");
    
    int count = cJSON_GetArraySize(datas);
    for (int i = 0; i < count; i++) {
        
        FRIEND_INFO info;
        cJSON* data = cJSON_GetArrayItem(datas, i);
        
//        userJson.put("levelpoint", friend.getLEVEL_POINT());
//        userJson.put("total_hour", friend.getTOTAL_HOUR());
//        userJson.put("total_cleanpot", friend.getTOTAL_CLEANPOT());
//        userJson.put("total_earning", friend.getTOTAL_EARNING());
//        userJson.put("total_tournament", friend.getTOTAL_TOURNAMENT());
//        userJson.put("total_emoticon", friend.getTOTAL_EMOTICON());
//        userJson.put("total_invite", friend.getTOTAL_INVITE());
//        userJson.put("total_avatars", friend.getAVATARS());
        
        info.user_id = cJSON_GetObjectItem(data, "user_id")->valuestring;
        //name
        info.user_name = cJSON_GetObjectItem(data, "name")->valuestring;
        //photo
        info.photo_id = cJSON_GetObjectItem(data, "photo_id")->valuestring;
        //points
        info.points = cJSON_GetObjectItem(data, "point")->valuedouble;
        //level
        info.level = cJSON_GetObjectItem(data, "level")->valueint;
        
        //total play
        info.total_play = cJSON_GetObjectItem(data, "totalplay")->valueint;
        //win play
        info.win_play = cJSON_GetObjectItem(data, "winplay")->valueint;
        //max earning
        info.max_earning = cJSON_GetObjectItem(data, "max_earning")->valuedouble;
        //win streak
        info.win_streak = cJSON_GetObjectItem(data, "win_streak")->valueint;
        
        g_vFriends.push_back(info);
    }
    
    cJSON_Delete(json);
    
    getGiftList();
    g_nLimitLoadingCount = 80;
}

//void WebService::getGlobalRankingList()
//{
//    std::string strUrl = StringUtils::format("http://%s:8080/sys/money/mobile/rankingGlobal.do", WEBSERVICE_HOST);
//    
//    HttpRequest* request = new HttpRequest();
//    request->setRequestType(HttpRequest::Type::POST);
//    request->setUrl(strUrl.c_str());
//    request->setResponseCallback(CC_CALLBACK_2(WebService::onGlobalRankingCompleted, this));
//    
////    // write the post data
////    std::string strData = StringUtils::format("USER_ID=%s",
////                                              g_mainUserInfo.fb_id.c_str()
////                                              );
////    request->setRequestData(strData.c_str(), strData.length());
//    
//    request->setTag("get Global Ranking");
//    //HttpClient::getInstance()->send(request);
//    HttpClient::getInstance()->sendImmediate(request);
//    
//    request->release();
//}
//
//void WebService::getFriendRankingList()
//{
//    std::string strUrl = StringUtils::format("http://%s:8080/sys/money/mobile/rankingFriends.do", WEBSERVICE_HOST);
//    
//    HttpRequest* request = new HttpRequest();
//    request->setRequestType(HttpRequest::Type::POST);
//    request->setUrl(strUrl.c_str());
//    request->setResponseCallback(CC_CALLBACK_2(WebService::onFriendRankingCompleted, this));
//    
//    //    // write the post data
//    //    std::string strData = StringUtils::format("USER_ID=%s",
//    //                                              g_mainUserInfo.fb_id.c_str()
//    //                                              );
//    //    request->setRequestData(strData.c_str(), strData.length());
//    
//    request->setTag("get Global Ranking");
//    //HttpClient::getInstance()->send(request);
//    HttpClient::getInstance()->sendImmediate(request);
//    
//    request->release();
//}
//
//void WebService::onGlobalRankingCompleted(HttpClient* sender, HttpResponse* response)
//{
//    if (response->isSucceed() == false){
//        
//        return;
//    }
//    
//    std::vector<char>buffer = *response->getResponseData();
//    std::string str_msg = &buffer[0];
//    
//    //parsing data
//    cJSON *json = cJSON_Parse(str_msg.c_str());
//    if (json == NULL) {
//        
//        return;
//    }
//    
//    cJSON *jsonRes = cJSON_GetObjectItem(json, "retcode");
//    if (jsonRes->valueint != 0) {
//        
//        return;
//    }
//    
//    g_vGlobalRanking.clear();
//    
//    cJSON* datas = cJSON_GetObjectItem(json, "ranks");
//    
//    int count = cJSON_GetArraySize(datas);
//    for (int i = 0; i < count; i++) {
//        
//        RANK_INFO info;
//        cJSON* data = cJSON_GetArrayItem(datas, i);
//        
//        //user id
//        info.user_id = cJSON_GetObjectItem(data, "userid")->valuestring;
//        //name
//        info.user_name = cJSON_GetObjectItem(data, "name")->valuestring;
//        //photo
//        info.photo_id = cJSON_GetObjectItem(data, "photo_id")->valuestring;
//        //diamonds
//        info.level = cJSON_GetObjectItem(data, "level")->valueint;
//        //bonus
//        info.total_earning = cJSON_GetObjectItem(data, "total_earning")->valuedouble;
//        
//        g_vGlobalRanking.push_back(info);
//    }
//    
//    cJSON_Delete(json);
//}
//
//void WebService::onFriendRankingCompleted(HttpClient* sender, HttpResponse* response)
//{
//    if (response->isSucceed() == false){
//        
//        return;
//    }
//    
//    std::vector<char>buffer = *response->getResponseData();
//    std::string str_msg = &buffer[0];
//    
//    //parsing data
//    cJSON *json = cJSON_Parse(str_msg.c_str());
//    if (json == NULL) {
//        
//        return;
//    }
//    
//    cJSON *jsonRes = cJSON_GetObjectItem(json, "retcode");
//    if (jsonRes->valueint != 0) {
//        
//        return;
//    }
//    
//    g_vFriendsRanking.clear();
//    
//    cJSON* datas = cJSON_GetObjectItem(json, "ranks");
//    
//    int count = cJSON_GetArraySize(datas);
//    for (int i = 0; i < count; i++) {
//        
//        RANK_INFO info;
//        cJSON* data = cJSON_GetArrayItem(datas, i);
//        
//        //user id
//        info.user_id = cJSON_GetObjectItem(data, "userid")->valuestring;
//        //name
//        info.user_name = cJSON_GetObjectItem(data, "name")->valuestring;
//        //photo
//        info.photo_id = cJSON_GetObjectItem(data, "photo_id")->valuestring;
//        //diamonds
//        info.level = cJSON_GetObjectItem(data, "level")->valueint;
//        //bonus
//        info.total_earning = cJSON_GetObjectItem(data, "total_earning")->valuedouble;
//        
//        g_vFriendsRanking.push_back(info);
//    }
//    
//    cJSON_Delete(json);
//}

void WebService::acceptGiftBonus(int index)
{
    std::string strUrl= StringUtils::format("http://%s:8080/sys/gift/mobile/acceptGift.do", WEBSERVICE_HOST);
    
    HttpRequest* request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::POST);
    request->setUrl(strUrl.c_str());
    request->setResponseCallback(NULL);
    
    // write the post data
    std::string strData = StringUtils::format("NO=%d",
                                              index
                                              );
    request->setRequestData(strData.c_str(), strData.length());
    
    request->setTag("accept Gift Bonus");
    //HttpClient::getInstance()->send(request);
    HttpClient::getInstance()->sendImmediate(request);
    
    request->release();
}

void WebService::acceptMsg(int index)
{
    std::string strUrl= StringUtils::format("http://%s:8080/sys/msg/mobile/acceptMsg.do", WEBSERVICE_HOST);
    
    HttpRequest* request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::POST);
    request->setUrl(strUrl.c_str());
    request->setResponseCallback(NULL);
    
    // write the post data
    std::string strData = StringUtils::format("NO=%d",
                                              index
                                              );
    request->setRequestData(strData.c_str(), strData.length());
    
    request->setTag("accept Msg");
    //HttpClient::getInstance()->send(request);
    HttpClient::getInstance()->sendImmediate(request);
    
    request->release();
}

void WebService::sendFriendRequest2FB(std::string friend_id)
{
    if(g_mainUserInfo.fb_id.empty() == true) return;
    
    std::string strUrl = StringUtils::format("http://%s:8080/sys/friend/mobile/requestFriend2.do", WEBSERVICE_HOST);
    
    HttpRequest* request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::POST);
    request->setUrl(strUrl.c_str());
    request->setResponseCallback(NULL);
    
    // write the post data
    std::string strData = StringUtils::format("USER_ID=%s&FRIEND_ID=%s&TYPE=0",
                                              g_mainUserInfo.user_id.c_str(),
                                              friend_id.c_str()
                                              );
    request->setRequestData(strData.c_str(), strData.length());
    
    request->setTag("request Friend for FB");
    //HttpClient::getInstance()->send(request);
    HttpClient::getInstance()->sendImmediate(request);
    
    request->release();
}

void WebService::sendFriendRequest2User(std::string friend_id)
{
    std::string strUrl = StringUtils::format("http://%s:8080/sys/friend/mobile/requestFriend.do", WEBSERVICE_HOST);
    
    HttpRequest* request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::POST);
    request->setUrl(strUrl.c_str());
    request->setResponseCallback(NULL);
    
    // write the post data
    std::string strData = StringUtils::format("USER_ID=%s&FRIEND_ID=%s&TYPE=1",
                                              g_mainUserInfo.user_id.c_str(),
                                              friend_id.c_str()
                                              );
    request->setRequestData(strData.c_str(), strData.length());
    
    request->setTag("request Friend for User");
    //HttpClient::getInstance()->send(request);
    HttpClient::getInstance()->sendImmediate(request);
    
    request->release();
}

//invite friend to game table
void WebService::inviteFriend2Table(std::string friend_id, std::string room_id, int position, std::string friend_name)
{
    std::string strUrl = StringUtils::format("http://%s:8080/sys/msg/mobile/sendMsg.do", WEBSERVICE_HOST);
    
    HttpRequest* request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::POST);
    request->setUrl(strUrl.c_str());
    //request->setResponseCallback(NULL);
    request->setResponseCallback(CC_CALLBACK_2(WebService::onInviteFriend2TableCompleted, this, friend_name));
    
    time_t cur_time;
    time(&cur_time);
    
    std::string strMsg = StringUtils::format("%.0f:%d", g_nBuyin, g_nPlayFee);
    
    // write the post data
    std::string strData = StringUtils::format("SENDER_ID=%s&RECEIVER_ID=%s&TITLE=%s&MSG=%s&SENT_TIME=%ld&TYPE=2",
                                              g_mainUserInfo.user_id.c_str(),
                                              friend_id.c_str(),
                                              room_id.c_str(),
                                              strMsg.c_str(),
                                              cur_time
                                              );
    request->setRequestData(strData.c_str(), strData.length());
    
    request->setTag("Invite Friend to Table");
    //HttpClient::getInstance()->send(request);
    HttpClient::getInstance()->sendImmediate(request);
    
    request->release();
}

void WebService::onInviteFriend2TableCompleted(HttpClient* sender, HttpResponse* response, std::string friend_name)
{
    if (response->isSucceed() == false) return;
    
    std::vector<char>buffer = *response->getResponseData();
    std::string str_msg = &buffer[0];
    
    //parsing data
    cJSON *json = cJSON_Parse(str_msg.c_str());
    if (json == NULL) {
        
        return;
    }
    
    cJSON *jsonRes = cJSON_GetObjectItem(json, "retcode");
    if (jsonRes->valueint != 0) {
        
        //cocos2d::MessageBox(StringUtils::format("Invitation for %s is failed !", friend_name.c_str()).c_str(), "Notification");
        CCLOG("Failed invite to table");
        return;
    }
    
    CCLOG("Success invite to table");
    
    return;
}

void WebService::updatePlayInfo(int earning, int playtime, int cleanpot, int streak)
{
    std::string strUrl = StringUtils::format("http://%s:8080/sys/member/mobile/updatePlay.do", WEBSERVICE_HOST);
    
    HttpRequest* request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::POST);
    request->setUrl(strUrl.c_str());
    request->setResponseCallback(NULL);
    
    // write the post data
    std::string strData = StringUtils::format("USER_ID=%s&POINT=%.0f&LEVEL=%d&LEVEL_POINT=%d&EARNING=%d&PLAYTIME=%d&CLEANPOT=%d&STREAK=%d",
                                              g_mainUserInfo.user_id.c_str(),
                                              g_mainUserInfo.points,
                                              g_mainUserInfo.level,
                                              g_mainUserInfo.level_point,
                                              earning,
                                              playtime,
                                              cleanpot,	
                                              streak
                                              );
    request->setRequestData(strData.c_str(), strData.length());
    
    request->setTag("Update Play");
    //HttpClient::getInstance()->send(request);
    HttpClient::getInstance()->sendImmediate(request);
    
    request->release();
}

void WebService::updateAvatar()
{
    std::string strUrl = StringUtils::format("http://%s:8080/sys/member/mobile/updateAvatars.do", WEBSERVICE_HOST);
    
    HttpRequest* request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::POST);
    request->setUrl(strUrl.c_str());
    request->setResponseCallback(NULL);
    
    // write the post data
    std::string strData = StringUtils::format("USER_ID=%s&NAME=%s&PHOTO_ID=%s&POINT=%.0f&AVATARS=%d",
                                              g_mainUserInfo.user_id.c_str(),
                                              g_mainUserInfo.user_name.c_str(),
                                              g_mainUserInfo.photo_id.c_str(),
                                              g_mainUserInfo.points,
                                              g_mainUserInfo.total_avatars
                                              );
    request->setRequestData(strData.c_str(), strData.length());
    
    request->setTag("Update Photo");
    //HttpClient::getInstance()->send(request);
    HttpClient::getInstance()->sendImmediate(request);
    
    request->release();
}

void WebService::updateEmoticon()
{
    std::string strUrl = StringUtils::format("http://%s:8080/sys/member/mobile/updateEmoticon.do", WEBSERVICE_HOST);
    
    HttpRequest* request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::POST);
    request->setUrl(strUrl.c_str());
    request->setResponseCallback(NULL);
    
    // write the post data
    std::string strData = StringUtils::format("USER_ID=%s&POINT=%.0f&TOTAL_EMOTICON=%d",
                                              g_mainUserInfo.user_id.c_str(),
                                              g_mainUserInfo.points,
                                              g_mainUserInfo.total_emoticon
                                              );
    request->setRequestData(strData.c_str(), strData.length());
    
    request->setTag("Update Emoticon");
    //HttpClient::getInstance()->send(request);
    HttpClient::getInstance()->sendImmediate(request);
    
    request->release();
}

void WebService::updatePoint()
{
    std::string strUrl = StringUtils::format("http://%s:8080/sys/member/mobile/updatePoint.do", WEBSERVICE_HOST);
    
    HttpRequest* request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::POST);
    request->setUrl(strUrl.c_str());
    request->setResponseCallback(NULL);
    
    // write the post data
    std::string strData = StringUtils::format("USER_ID=%s&POINT=%.0f",
                                              g_mainUserInfo.user_id.c_str(),
                                              g_mainUserInfo.points
                                              );
    request->setRequestData(strData.c_str(), strData.length());
    
    request->setTag("Update Diamond");
    //HttpClient::getInstance()->send(request);
    HttpClient::getInstance()->sendImmediate(request);
    
    request->release();

}

void WebService::updateMsgGiftInfo()
{
    std::string strUrl = StringUtils::format("http://%s:8080/sys/member/mobile/updateAdminInfo.do", WEBSERVICE_HOST);
    
    HttpRequest* request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::POST);
    request->setUrl(strUrl.c_str());
    request->setResponseCallback(NULL);
    
    // write the post data
    std::string strData = StringUtils::format("USER_ID=%s&LAST_MSG=%d&LAST_GIFT=%d",
                                              g_mainUserInfo.user_id.c_str(),
                                              g_mainUserInfo.last_msg,
                                              g_mainUserInfo.last_gift
                                              );
    request->setRequestData(strData.c_str(), strData.length());
    
    request->setTag("Update AdminInfo");
    //HttpClient::getInstance()->send(request);
    HttpClient::getInstance()->sendImmediate(request);
    
    request->release();
}

void WebService::updatePush()
{
    std::string strUrl = StringUtils::format("http://%s:8080/sys/member/mobile/updatePush.do", WEBSERVICE_HOST);
    
    HttpRequest* request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::POST);
    request->setUrl(strUrl.c_str());
    request->setResponseCallback(NULL);
    
    // write the post data
    std::string strData = StringUtils::format("USER_ID=%s&PUSH_FLAG=%d",
                                              g_mainUserInfo.user_id.c_str(),
                                              g_mainUserInfo.push_flag
                                              );
    request->setRequestData(strData.c_str(), strData.length());
    
    request->setTag("Update Push");
    //HttpClient::getInstance()->send(request);
    HttpClient::getInstance()->sendImmediate(request);
    
    request->release();
}

void WebService::updateFreeSpin()
{
    if (g_mainUserInfo.user_id.empty() == true) {
        
        return;
    }
    
    std::string strUrl = StringUtils::format("http://%s:8080/sys/member/mobile/updateFreeSpin.do", WEBSERVICE_HOST);
    
    HttpRequest* request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::POST);
    request->setUrl(strUrl.c_str());
    request->setResponseCallback(NULL);
    
    // write the post data
    std::string strData = StringUtils::format("USER_ID=%s&FREE_SPIN=%d&LAST_SPINTIME=%ld",
                                              g_mainUserInfo.user_id.c_str(),
                                              g_nFreeSpin,
                                              g_timeLastFreeSpin
                                              );
    request->setRequestData(strData.c_str(), strData.length());
    
    request->setTag("Update FreeSpin");
    //HttpClient::getInstance()->send(request);
    HttpClient::getInstance()->sendImmediate(request);
    
    request->release();
}

void WebService::updateToken()
{
    if (g_mainUserInfo.user_id.empty() == true) {
        
        return;
    }
    
    std::string strUrl = StringUtils::format("http://%s:8080/sys/member/mobile/updateToken.do", WEBSERVICE_HOST);
    
    HttpRequest* request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::POST);
    request->setUrl(strUrl.c_str());
    request->setResponseCallback(NULL);
    
    int dev_type;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    
    dev_type = 1;
#else if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
    dev_type = 2;
#endif
    // write the post data
    std::string strData = StringUtils::format("USER_ID=%s&TOKEN=%s&DEV_TYPE=%d",
                                              g_mainUserInfo.user_id.c_str(),
                                              g_strDeviceToken.c_str(),
                                              dev_type
                                              );
    request->setRequestData(strData.c_str(), strData.length());
    
    request->setTag("Update Token");
    //HttpClient::getInstance()->send(request);
    HttpClient::getInstance()->sendImmediate(request);
    
    request->release();
}

void WebService::setProfileImage(std::string photo_id, ImageView *pImgView)
{
    //set default image, first
    pImgView->loadTexture("img_mask.png", TextureResType::PLIST);
    pImgView->setVisible(true);
    
    if (photo_id.empty() == true) {
        
        return;
    }
    
    int len = photo_id.length();
    if(len < 6) {
        
        pImgView->loadTexture(StringUtils::format("avatar_%s.jpg", photo_id.c_str()), TextureResType::PLIST);
        return;
    }
    
    std::string url = StringUtils::format("https://graph.facebook.com/%s/picture?height=128&width=128", photo_id.c_str());
    setImageFromUrl(url, pImgView);
    
//    std::string photo_file = FileUtils::getInstance()->fullPathForFilename(photo_id+".jpg");
//    CCLOG("WebService::setProfileImage - %s, %s", photo_id.c_str(), photo_file.c_str());
//
//    //check if file is exist, first
//    if(photo_file.empty() == false) {
//        
//        long szFile = FileUtils::getInstance()->getFileSize(photo_file);
//        if (szFile > 0)
//        {
//            
//            pImgView->loadTexture(photo_file);
//            //pImgView->loadTexture(photo_id+".jpg");
//            return;
//        }
//    }
//    
//    char url[1024] = "";
//    //sprintf(url, "https://graph.facebook.com/%s/picture?type=small", user_id.c_str());
//    sprintf(url, "https://graph.facebook.com/%s/picture?height=128&width=128", photo_id.c_str());
//    
//    HttpRequest* request = new HttpRequest();
//    request->setRequestType(HttpRequest::Type::GET);
//    request->setUrl(url);
//    request->setResponseCallback(CC_CALLBACK_2(WebService::onImgDownloadCompleted, this, photo_id, pImgView));
//    request->setTag("Get Profile Image");
//    //HttpClient::getInstance()->send(request);
//    HttpClient::getInstance()->sendImmediate(request);
//    
//    request->release();
    
    return;
}

void WebService::onImgDownloadCompleted(HttpClient* sender, HttpResponse* response, std::string  str_id, ImageView *pView)
{
    if (response->isSucceed() == false) return;
    
    std::string path = FileUtils::getInstance()->getWritablePath();
    std::string file_name = str_id + ".jpg";
    std::string file_path = path+file_name;
    
    FILE* fp = fopen(file_path.c_str(), "w+b");
    
    if (fp == NULL)
        return;
    
    std::vector<char> *buffer = response->getResponseData();
    unsigned char bf;
    for (unsigned int i = 0; i < buffer->size(); i++) {
        
        bf = buffer->at(i);
        fwrite(&bf, 1, 1, fp);
    }
    fclose(fp);
    
    if(pView != NULL) {
     
        pView->loadTexture(file_name);
    }
}




void WebService::getGiftList()
{
    std::string strUrl = StringUtils::format("http://%s:8080/sys/gift/mobile/getUserGifts.do", WEBSERVICE_HOST);
    
    HttpRequest* request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::POST);
    request->setUrl(strUrl.c_str());
    request->setResponseCallback(CC_CALLBACK_2(WebService::onGiftListDownloadCompleted, this));
    
    // write the post data
    std::string strData = StringUtils::format("RECEIVER_ID=%s", g_mainUserInfo.user_id.c_str());
    request->setRequestData(strData.c_str(), strData.length());
    request->setTag("Get Gift List");
    //HttpClient::getInstance()->send(request);
    HttpClient::getInstance()->sendImmediate(request);
    
    request->release();
}

void WebService::onGiftListDownloadCompleted(HttpClient* sender, HttpResponse* response)
{
    if (response->isSucceed() == false){
        
        return;
    }
    
    std::vector<char>buffer = *response->getResponseData();
    std::string str_msg = &buffer[0];
    
    //parsing data
    cJSON *json = cJSON_Parse(str_msg.c_str());
    if (json == NULL) {
        
        return;
    }
    
    cJSON *jsonRes = cJSON_GetObjectItem(json, "retcode");
    if (jsonRes->valueint != 0) {
        
        return;
    }
    
    cJSON* datas = cJSON_GetObjectItem(json, "gifts");
    
    int count = cJSON_GetArraySize(datas);
    int num = 0;
    for (int i = 0; i < count; i++) {
        
        cJSON* data = cJSON_GetArrayItem(datas, i);
        
        GIFT_INFO info;
        
        //no
        info.index = cJSON_GetObjectItem(data, "no")->valueint;
        //type
        info.type = cJSON_GetObjectItem(data, "type")->valueint;
        //bonus
        info.bonus = cJSON_GetObjectItem(data, "bonus")->valuedouble;
        
        bool flag = addGift(info);
        if (flag == true) {
            
            num++;
        }
    }
    cJSON_Delete(json);
    
    getMsgList();
    g_nLimitLoadingCount = 90;
}

void WebService::getMsgList()
{
    std::string strUrl = StringUtils::format("http://%s:8080/sys/msg/mobile/getUserMsgs.do", WEBSERVICE_HOST);
    
    HttpRequest* request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::POST);
    request->setUrl(strUrl.c_str());
    request->setResponseCallback(CC_CALLBACK_2(WebService::onMsgListDownloadCompleted, this));
    
    // write the post data
    std::string strData = StringUtils::format("RECEIVER_ID=%s", g_mainUserInfo.user_id.c_str());
    request->setRequestData(strData.c_str(), strData.length());
    request->setTag("Get Msg List");
    //HttpClient::getInstance()->send(request);
    HttpClient::getInstance()->sendImmediate(request);
    
    request->release();
}

void WebService::onMsgListDownloadCompleted(HttpClient* sender, HttpResponse* response)
{
    if (response->isSucceed() == false){
        
        return;
    }
    
    std::vector<char>buffer = *response->getResponseData();
    std::string str_msg = &buffer[0];
    
    //parsing data
    cJSON *json = cJSON_Parse(str_msg.c_str());
    if (json == NULL) {
        
        return;
    }
    
    cJSON *jsonRes = cJSON_GetObjectItem(json, "retcode");
    if (jsonRes->valueint != 0) {
        
        return;
    }
    
    cJSON* datas = cJSON_GetObjectItem(json, "msgs");
    
    int count = cJSON_GetArraySize(datas);
    for (int i = 0; i < count; i++) {
        
        cJSON* data = cJSON_GetArrayItem(datas, i);
        
        MSG_INFO info;
        
        //no
        info.index = cJSON_GetObjectItem(data, "no")->valueint;
        //type
        info.type = cJSON_GetObjectItem(data, "type")->valueint;
        //sender id
        info.sender_id = cJSON_GetObjectItem(data, "sender_id")->valuestring;
        //name
        info.name = cJSON_GetObjectItem(data, "name")->valuestring;
        //photo
        info.photo_id = cJSON_GetObjectItem(data, "photo")->valuestring;
        //title
        info.title = cJSON_GetObjectItem(data, "title")->valuestring;
        //msg
        info.msg = cJSON_GetObjectItem(data, "msg")->valuestring;
        //time
        info.sent_time = cJSON_GetObjectItem(data, "time")->valueint;
        
        
        g_vMessages.push_back(info);
    }
    cJSON_Delete(json);
    
    updateToken();
    
    time_t cur_time;
    time(&cur_time);
    
    double passSeconds = difftime(cur_time, g_timeLastFreeSpin);
    int num = passSeconds/(24*3600);
    if (num > 0) {
        
        g_nFreeSpin += num;
        g_timeLastFreeSpin = cur_time;
        updateFreeSpin();
    }
    
    g_nLimitLoadingCount = 100;
}


