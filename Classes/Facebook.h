#ifndef __FACEBOOK_H_
#define __FACEBOOK_H_

#include "common.h"
#include "network/HttpClient.h"

#include "FacebookAgent.h"

using namespace cocos2d::network;
using namespace cocos2d::plugin;

class ProfileLayer;

extern int g_nFBLoginType;;

class Facebook
{
public:
    
    static Facebook* getInstance();
    
    Facebook();
    ~Facebook();
    
    void login();
    void logout();
    bool isLogined();
    
    const char* getUserId();
    const char* getUserName();
    const char* getUserImg();
    
    void getFBFriends();
    void sendInviteRequest(std::string frined_ids);
    
    void shareGame();
    void inviteFriend();
    
    void getFacebookInfo();
    void getFacebookUserName();
    
    void showErrorMsg();
    void showInviteDoneMsg();
    void switchScene();
    
    void setProfileLayer(ProfileLayer *profileLayer){ pProfileLayer = profileLayer; }
    
    void onLoginDone(int ret, std::string& msg);
    void onGetUserNameDone(int ret, std::string& msg);
    void onGetFriendsDone(int ret, std::string& str_msg);
    
private:
    
    std::string fb_user_id;
    std::string fb_user_name;
    std::string fb_token;
    
    char strFBImage[1024];
    bool isLoginProcessing;
    
    ProfileLayer *pProfileLayer;
    
    static Facebook* _instance;
};

#endif  //__FACEBOOK_H_
