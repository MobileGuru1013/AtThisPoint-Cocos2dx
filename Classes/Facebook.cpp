
#include "Facebook.h"

#include "common.h"
#include "appwarp.h"
#include "WebService.h"

#include "HelloWorldScene.h"

#define L_APPNAME           "Point Game"
#define L_SHAREGAME_MSG     "Point Game is great card game !!!"

int g_nFBLoginType = 0;

Facebook* Facebook::_instance = NULL;

Facebook* Facebook::getInstance()
{
    if (_instance == NULL) {
        
        _instance = new Facebook();
        _instance->fb_user_name = "You";
    }
    
    return _instance;
}

Facebook::Facebook()
{
    isLoginProcessing = false;
    
    fb_user_id = cocos2d::UserDefault::getInstance()->getStringForKey("fb_id");
}

Facebook::~Facebook()
{
    
}

void Facebook::login()
{
    if(isLoginProcessing == true) return;
    
    isLoginProcessing = true;
    
    if(FacebookAgent::getInstance()->isLoggedIn())
    {
        CCLOG("Already login");
        getFacebookInfo();
    }
    else
    {
        //std::string permissions = "user_friends, user_games_activity, publish_actions";
        std::string permissions = "public_profile, user_friends";
        FacebookAgent::getInstance()->login(permissions, CC_CALLBACK_2(Facebook::onLoginDone, this));
    }
}

void Facebook::logout()
{
    FacebookAgent::getInstance()->logout();
}

void Facebook::onLoginDone(int ret, std::string& msg)
{
    if (ret == 0) { //if login success
        
        //std::string permissions = FacebookAgent::getInstance()->getPermissionList();
        
        auto scheduler = cocos2d::Director::getInstance()->getScheduler();
        scheduler->performFunctionInCocosThread(CC_CALLBACK_0(Facebook::getFacebookInfo, this));
        
        CCLOG("onLoginDone: suceess");
    }
    else {
        
        CCLOG("onLoginDone: failed");
        
        auto scheduler = cocos2d::Director::getInstance()->getScheduler();
        scheduler->performFunctionInCocosThread(CC_CALLBACK_0(Facebook::showErrorMsg, this));
        
        isLoginProcessing = false;
    }
}

void Facebook::showErrorMsg()
{
    //helloScene->showErrorMsg();
}

void Facebook::getFacebookInfo()
{
    getFacebookUserName();
    
//    if(g_nFBLoginType == 0) { //profile login
//        
//        sprintf(strFBImage, "");
//        getFacebookUserName();
//    }
//    else if (g_nFBLoginType == 1) { //invite
//        
//        //inviteFriend();
//        getFBFriends();
//    }
}

bool Facebook::isLogined()
{
    return false;
}

const char* Facebook::getUserId()
{
    return fb_user_id.c_str();
}

const char* Facebook::getUserName()
{
    return fb_user_name.c_str();
}

const char* Facebook::getUserImg()
{
    return NULL;
}

void Facebook::inviteFriend()
{
    std::string strTitle = StringUtils::format("%s invited you to play Cuarenta Game", g_mainUserInfo.user_name.c_str());
    FacebookAgent::FBInfo params;
    params.insert(std::make_pair("message", "Hello, let's play Cuarenta Game with me !"));
    params.insert(std::make_pair("title", strTitle));
    //params.insert(std::make_pair("redirect_uri", "https://fb.me/188149974896346"));
    
    FacebookAgent::getInstance()->appRequest(params, [=](int ret, std::string& msg){
        
        isLoginProcessing = false;
        CCLOG("ret = %d, %s", ret, msg.c_str());
        if (ret == 0) {
            
            cJSON *json = cJSON_Parse(msg.c_str());
            if (json == NULL) return;
            
            int cntFriends = 0;
            cJSON *jsonTo = cJSON_GetObjectItem(json, "to");
            if(jsonTo == NULL) return;
            
            cJSON *begPtr = jsonTo;
            jsonTo = jsonTo->child;
            while(jsonTo != NULL)
            {
                std::string friend_id = jsonTo->valuestring;
                CCLOG("friend : %s", friend_id.c_str());
                
                //call web service for friend request
                WebService::getInstance()->sendFriendRequest2FB(friend_id);
                
                cntFriends++;
                jsonTo = jsonTo->next;
            }
            cJSON_Delete(begPtr);
            
            if (cntFriends == 0) return;
            
            //float bonus_cash = 50000*cntFriends;
            //g_mainUserInfo.diamonds += bonus_cash;
          
            //            auto scheduler = cocos2d::Director::getInstance()->getScheduler();
            //            scheduler->performFunctionInCocosThread(CC_CALLBACK_0(Facebook::showInviteDoneMsg, this));
        }
    });
}

void Facebook::showInviteDoneMsg()
{
    //shareScene->showInfoMsg();
}

void Facebook::getFacebookUserName()
{
    //get user name
    //std::string path = "/me?fields=first_name";
    std::string path = "/me";
    
    //std::string path = fb_user_id;
    FacebookAgent::FBInfo params;
    params.insert(std::make_pair("fields", "first_name"));
    FacebookAgent::getInstance()->api(path, FacebookAgent::HttpMethod::Get, params, CC_CALLBACK_2(Facebook::onGetUserNameDone, this));
}

void Facebook::onGetUserNameDone(int ret, std::string& str_msg)
{
    if (ret == 0) {
        
        CCLOG("onGetUserNameDone: success");
        //parsing data
        cJSON *json = cJSON_Parse(str_msg.c_str());
        char* userName;
        cJSON *jsonName = cJSON_GetObjectItem(json, "first_name");
        if (jsonName == NULL) {
            
            userName = cJSON_GetObjectItem(json, "name")->valuestring;
        }
        else {
            
            userName = jsonName->valuestring;
        }
        
        printf("UserName = %s\n", userName);
        fb_user_name = userName;
        tmp_fb_username = userName;
        char* userId = cJSON_GetObjectItem(json, "id")->valuestring;
        printf("UserId = %s\n", userId);
        fb_user_id = userId;
        
        //save facebook info
//        g_mainUserInfo.fb_id = fb_user_id;
//        g_mainUserInfo.user_name = fb_user_name;
//        g_mainUserInfo.photo_id = g_mainUserInfo.fb_id;
//        saveProfileData();
        
        getFBFriends();
        
//        auto scheduler = cocos2d::Director::getInstance()->getScheduler();
//        // send message to UI thread
//        scheduler->performFunctionInCocosThread(CC_CALLBACK_0(Facebook::switchScene, this));
    }
    else {
        
        //g_tmpErrorMsg = str_msg;
        CCLOG("onGetUserNameDone: failed");
        CCLOG("%s", str_msg.c_str());
        auto scheduler = cocos2d::Director::getInstance()->getScheduler();
        scheduler->performFunctionInCocosThread(CC_CALLBACK_0(Facebook::showErrorMsg, this));
        
        //helloScene->stopLoadingAnimation();
        isLoginProcessing = false;
    }
}

void Facebook::switchScene()
{
    //update profile with facebook info
    g_pHelloWorld->onFBConnectDone();
    isLoginProcessing = false;
}

void Facebook::shareGame()
{
    FacebookAgent::FBInfo params;
    
    params.insert(std::make_pair("dialog", "shareLink"));
    params.insert(std::make_pair("description", L_SHAREGAME_MSG));
    params.insert(std::make_pair("title", L_APPNAME));
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
    params.insert(std::make_pair("link", "https://play.google.com/store/apps/details?id=com.xinxing.balootonline"));
#else
    
    params.insert(std::make_pair("link", "https://itunes.apple.com/app/id1011786150"));
#endif
    
    if (FacebookAgent::getInstance()->canPresentDialogWithParams(params))
    {
        //CCLOG("ShareLink dialog\n");
        FacebookAgent::getInstance()->dialog(params, [=](int ret, std::string& msg){
            
            //CCLOG("ShareGame_1 : ret = %d, %s\n", ret, msg.c_str());
        });
    }
    else {
        
        // CCLOG("Share function\n");
        
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        
        FacebookAgent::getInstance()->webDialog(params, [=](int ret, std::string& msg){
            
            // CCLOG("ShareGame_1 : ret = %d, %s\n", ret, msg.c_str());
        });
#else
        
        FacebookAgent::getInstance()->share(params, [=](int ret, std::string& msg){
            
            // CCLOG("ShareGame_2 : ret = %d, %s\n", ret, msg.c_str());
        });
#endif
        
    }
}

void Facebook::getFBFriends()
{
    if(fb_user_id.empty() == true) return;
    
    g_vFBFriends.clear();
    
    std::string path = "/me";
    //std::string path = "/me/friends?fields=installed";
    
    //std::string path = fb_user_id;
    FacebookAgent::FBInfo params;
    params.insert(std::make_pair("fields", "invitable_friends.limit(1000)"));
    // params.insert(std::make_pair("limit", "1000"));
    
    FacebookAgent::getInstance()->api(path, FacebookAgent::HttpMethod::Get, params, CC_CALLBACK_2(Facebook::onGetFriendsDone, this));
}

void Facebook::onGetFriendsDone(int ret, std::string& str_msg)
{
    if (ret == 0) {
        
        CCLOG("onGetFriendsDone: success");
        //parsing data
        cJSON *json = cJSON_Parse(str_msg.c_str());
        cJSON *jsonFriends = cJSON_GetObjectItem(json, "invitable_friends");
        if (jsonFriends == NULL) {
            
            return;
        }
        
        cJSON* datas = cJSON_GetObjectItem(jsonFriends, "data");
        int count = cJSON_GetArraySize(datas);
        for (int i = 0; i < count; i++) {
            
            cJSON* data = cJSON_GetArrayItem(datas, i);
            
            char *strId = cJSON_GetObjectItem(data, "id")->valuestring;
            char *strName = cJSON_GetObjectItem(data, "name")->valuestring;
            
            cJSON* jsonPicture = cJSON_GetObjectItem(data, "picture");
            cJSON* jsonPictureData = cJSON_GetObjectItem(jsonPicture, "data");
            char *strPicture = cJSON_GetObjectItem(jsonPictureData, "url")->valuestring;
            
            FRIEND_INFO info;
            info.fb_id = strId;
            info.user_name = strName;
            info.photo_id = strPicture;
            
            g_vFBFriends.push_back(info);
        }
        
        cJSON_Delete(json);
        
        auto scheduler = cocos2d::Director::getInstance()->getScheduler();
        // send message to UI thread
        scheduler->performFunctionInCocosThread(CC_CALLBACK_0(Facebook::switchScene, this));
    }
    else {
        
        //g_tmpErrorMsg = str_msg;
        CCLOG("onGetUserNameDone: failed");
        CCLOG("%s", str_msg.c_str());
        auto scheduler = cocos2d::Director::getInstance()->getScheduler();
        scheduler->performFunctionInCocosThread(CC_CALLBACK_0(Facebook::showErrorMsg, this));
        
        //helloScene->stopLoadingAnimation();
        isLoginProcessing = false;
    }
}

void Facebook::sendInviteRequest(std::string frined_ids)
{
    std::string strTitle = StringUtils::format("Invitation for At Point game ", g_mainUserInfo.user_name.c_str());
    FacebookAgent::FBInfo params;
    params.insert(std::make_pair("message", "Hello, let's play At Point Game with me !"));
    //params.insert(std::make_pair("title", strTitle));
    params.insert(std::make_pair("to", frined_ids));
    //params.insert(std::make_pair("action_type", "send"));
    //params.insert(std::make_pair("object_id", fb_user_id));
    //params.insert(std::make_pair("redirect_uri", "https://fb.me/188149974896346"));
    
    FacebookAgent::getInstance()->appRequest(params, [=](int ret, std::string& msg){
        
        isLoginProcessing = false;
        CCLOG("ret = %d, %s", ret, msg.c_str());
        if (ret == 0) {
            
            cJSON *json = cJSON_Parse(msg.c_str());
            if (json == NULL) return;
            
            //            cJSON *jsonRequest = cJSON_GetObjectItem(json, "request");
            //            char* sendId = jsonRequest->valuestring;
            //
            int cntFriends = 0;
            cJSON *jsonTo = cJSON_GetObjectItem(json, "to");
            if(jsonTo == NULL) return;
            
            cJSON *begPtr = jsonTo;
            jsonTo = jsonTo->child;
            while(jsonTo != NULL)
            {
                std::string friend_id = jsonTo->valuestring;
                CCLOG("friend : %s", friend_id.c_str());
                
                //call web service for friend request
                MessageBox("Request Sent", "");
                
                WebService::getInstance()->sendFriendRequest2FB(friend_id);
                
                cntFriends++;
                jsonTo = jsonTo->next;
            }
            cJSON_Delete(begPtr);
            
            if (cntFriends == 0) return;
            
            //float bonus_cash = 50000*cntFriends;
            //g_mainUserInfo.diamonds += bonus_cash;
            
            //            auto scheduler = cocos2d::Director::getInstance()->getScheduler();
            //            scheduler->performFunctionInCocosThread(CC_CALLBACK_0(Facebook::showInviteDoneMsg, this));
        }
    });
}

