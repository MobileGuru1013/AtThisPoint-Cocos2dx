
#ifndef WebService_hpp
#define WebService_hpp

#include "common.h"

//typedef std::function<void(PROFILEINFO* client)> fProfileCallback;

class WebService : public cocos2d::Node {
    
public:
    
    WebService();
    ~WebService();
    
    static WebService* getInstance();
    
    void login2Guest();
    void login2FB(std::string fb_id);
    
    void getUserProfile(std::string user_id);
    
    void sendFriendRequest2FB(std::string friend_id);
    void sendFriendRequest2User(std::string friend_id);
    
    void inviteFriend2Table(std::string friend_id, std::string room_id, int position, std::string friend_name);
    void getFriendList();
    void getGiftList();
    void getMsgList();
    void getAdsImage();
    
    void acceptGiftBonus(int index);
    void acceptMsg(int index);
    
//    void getGlobalRankingList();
//    void getFriendRankingList();
    
    void updatePlayInfo(int earning, int playtime, int cleanpot, int streak);
    void updateAvatar();
    void updateEmoticon();
    void updatePoint();
    void updatePush();
    void updateToken();
    void updateMsgGiftInfo();
    void updateFreeSpin();
    
    void setProfileImage(std::string photo_id, ImageView *pImgView);

private:
    
    void onGuestLoginCompleted(HttpClient* sender, HttpResponse* response);
    void onFBLoginCompleted(HttpClient* sender, HttpResponse* response);
    void onUserProfileCompleted(HttpClient* sender, HttpResponse* response);
    
    void onFriendListCompleted(HttpClient* sender, HttpResponse* response);
    void onInviteFriend2TableCompleted(HttpClient* sender, HttpResponse* response, std::string friend_name);
    
    void onGiftListDownloadCompleted(HttpClient* sender, HttpResponse* response);
    void onMsgListDownloadCompleted(HttpClient* sender, HttpResponse* response);
    
//    void onGlobalRankingCompleted(HttpClient* sender, HttpResponse* response);
//    void onFriendRankingCompleted(HttpClient* sender, HttpResponse* response);
    
    void onImgDownloadCompleted(HttpClient* sender, HttpResponse* response, std::string  str_id, ImageView *pView);
    
    static WebService* _instance;
};

#endif /* WebService_hpp */
