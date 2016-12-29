

#include "HelloWorldScene.h"
#include "Facebook.h"
#include "TutorialScene.h"

#include "WebService.h"
#include "AppService.h"

//define string
std::string g_strHelloWorld[6][2] = {{"SELECT TABLE", "SELECCIONA UNA MESA"},
                                     {"TOURNAMENTS", "TORNEOS"},
                                     {"LEADERBOARD", "TABLA DE LIDERES"},
                                     {"Notification", "Notificación"},
                                     {"SOON", "PRONTO"},
                                     {"WIN PRIZES", "GANA PREMIOS"},
                                    };

std::string g_imgHelloWorld[2][2] = {{"soon.png", "_soon.png"},
                                     {"text_collect.png", "_text_collect.png"}
                                    };

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    TextureCache::getInstance()->removeUnusedTextures();
    
    AppService::hideCircleProgressbar();
    
    auto rootNode = CSLoader::createNode("MainScene.csb");
    addChild(rootNode);
    
    //bonus node
//    bonus_node = (Node*)rootNode->getChildByName("bonusNode");
//    bonus_node->setVisible(false);
//    bonus_node->setScaleX(g_fScaleWH);
//    auto panel_effect = (Layout*)bonus_node->getChildByName("panel_effect");
//    panel_effect->addTouchEventListener(CC_CALLBACK_2(HelloWorld::onMainBtnClicked, this, 8));
//    auto img_icon = (ImageView*)panel_effect->getChildByName("icon");
//    img_icon->loadTexture("bonus_icon.png", TextureResType::PLIST);
//    
//    pNodeAnimation = CSLoader::createTimeline("SpinWinNode.csb");
//    pNodeAnimation->retain();
//    pNodeAnimation->gotoFrameAndPause(0);
    
    // img_soon = (ImageView*)rootNode->getChildByName("Image_1");
    
    lb_title[0] = (Text*)rootNode->getChildByName("text_0");
  
    //main buttons
    for (int i = 0; i < 8; i++) {
        
        std::string strName = StringUtils::format("btn_%d", i);
        auto btnMain = static_cast<Button*>(rootNode->getChildByName(strName));
        btnMain->addTouchEventListener(CC_CALLBACK_2(HelloWorld::onMainBtnClicked, this, i));
        
        if(i == 0) {
            
            lb_points = (Text*)btnMain->getChildByName("lb_point");
            lb_points->setString("");
        }
        else if(i == 1) {
            
            btn_fb = btnMain;
        }
        else if(i == 2) {
            
            img_push = (ImageView*)btnMain->getChildByName("push_mark");
            img_push->setVisible(false);
        }
        else if (i == 5) {
            
            lb_title[1] = (Text*)btnMain->getChildByName("text");
            lb_ribbon_1 = (Text*)((Node*)btnMain->getChildByName("ribbon"))->getChildByName("text_0");
        }
        else if (i == 6) {
            
            lb_title[2] = (Text*)btnMain->getChildByName("text");
            lb_ribbon_2 = (Text*)((Node*)btnMain->getChildByName("ribbon"))->getChildByName("text_0");
        }
    }
    
    img_bonustimer = (ImageView*)rootNode->getChildByName("timer");
    img_bonustimer->setVisible(false);
    lb_bonustime = (TextBMFont*)img_bonustimer->getChildByName("lb_time");
    
    panel_bonus = (Layout*)rootNode->getChildByName("panel_bonus");
    panel_bonus->setVisible(false);
    panel_bonus->addTouchEventListener(CC_CALLBACK_2(HelloWorld::onMainBtnClicked, this, 8));
    
    panel_profile = (Layout*)rootNode->getChildByName("panel_0");
    panel_profile->addTouchEventListener(CC_CALLBACK_2(HelloWorld::onMainBtnClicked, this, 9));

    lb_name = (Text*)(rootNode->getChildByName("lb_name"));
    lb_name->setString("");
    lb_level = (Text*)(rootNode->getChildByName("lb_level"));
    lb_level->setString("");
    img_levelbar = (ImageView*)(rootNode->getChildByName("levelbar"));
    img_levelbar->setVisible(false);
    level_bar = (LoadingBar*)(img_levelbar->getChildByName("pro_bar"));
    
    //init SelectMode layer
//    pSelectModeLayer = new SelectModeLayer();
//    pSelectModeLayer->createLayer(this, 100);
    auto imgPhotoPanel = (ImageView*)(rootNode->getChildByName("photo"));
    imgPhotoPanel->setScaleX(g_fScaleWH);
    
    //clipping node
    auto clippingNode = ClippingNode::create();
    clippingNode->setAlphaThreshold(MASKING_THRESHOLD);
    clippingNode->setPosition(0, 0);
    clippingNode->setAnchorPoint(Vec2(0, 0));
    this->addChild(clippingNode);
    
    auto mask = Sprite::createWithSpriteFrameName("img_mask.png");
    float scaleX = (imgPhotoPanel->getBoundingBox().size.width) / (float)mask->getBoundingBox().size.width;
    float scaleY = (imgPhotoPanel->getBoundingBox().size.height) / (float)mask->getBoundingBox().size.height;
    mask->setScale(scaleX, scaleY);
    mask->setPosition(imgPhotoPanel->getPosition());
    clippingNode->setStencil(mask);
    
    img_photo = ImageView::create("img_mask.png", TextureResType::PLIST);
    img_photo->setScale(scaleX, scaleY);
    img_photo->setPosition(imgPhotoPanel->getPosition());
    clippingNode->addChild(img_photo);
    
    auto imgBorder = ImageView::create("border.png", TextureResType::PLIST);
    imgBorder->setScale(scaleX, scaleY);
    imgBorder->setPosition(imgPhotoPanel->getPosition());
    this->addChild(imgBorder);
    
    img_photo->setVisible(false);
   
    this->setKeyboardEnabled(true);
    g_pHelloWorld = this;
    
    //init alert layer
    pAlertLayer = new AlertLayer();
    pAlertLayer->createLayer(this, 150, this);
    
    //init private profile layer
    pPrivateProfileLayer = new PrivateProfileLayer();
    pPrivateProfileLayer->createLayer(this, 100, this);
    
    //init social layer
    pSocialLayer = new SocialLayer();
    pSocialLayer->createLayer(this, 100, this);
    
    //init leaderboard layer
    pLeaderboardLayer = new LeaderboardLayer();
    pLeaderboardLayer->createLayer(this, 100, this);
    
    //init spin layer
    pSpinLayer = new SpinLayer();
    pSpinLayer->createLayer(this, 100, this);
    
    g_pSpinLayer = pSpinLayer;
    
    //init store layer
    pStoreLayer = new StoreLayer();
    pStoreLayer->createLayer(this, 100, this);
    
    g_pStoreLayer = pStoreLayer;
    
    //init bonus layer
    pGiftBonusLayer = new GiftBonusLayer();
    pGiftBonusLayer->createLayer(this, 100, this);
    
    //init facebook connect layer
    pFBConnectLayer = new FBConnectLayer();
    pFBConnectLayer->createLayer(this, 100, this);
    
    //init facebook bonus layer
    pFBBonusLayer = new FBBonusLayer();
    pFBBonusLayer->createLayer(this, 100, this);
    //pFBBonusLayer->showLayer();
    
    //init facebook invite layer
    pFBInviteLayer = new FBInviteLayer();
    pFBInviteLayer->createLayer(this, 100, this);
    //pFBInviteLayer->showLayer();
    
    //init setting layer
    pSettingLayer = new SettingLayer();
    pSettingLayer->createLayer(this, 100, this);
    
    //init carousel layer
    pCarouselLayer = new CarouselLayer();
    pCarouselLayer->createLayer(this, 10);
    pCarouselLayer->showLayer();
    
    //init carousel
//    pCarousel = new Carousel();
//    pCarousel->create();
//    this->addChild(pCarousel, 150);
    
    showProfileInfo();
    
    if (getGiftBonusFlag() == true) {
        
        showBonusNode();
    }
    
    updateString2Lang();
    updatePushMark();
    
    if (cocos2d::UserDefault::getInstance()->getBoolForKey("lobby") == false) {
        
        cocos2d::UserDefault::getInstance()->setBoolForKey("lobby", true);
        if(g_nLangType == 0) {
         
            pAlertLayer->showLayer("", "You may change the app landing\n between lobby and table\n with QuickPlay feature");
        }
        else {
            
            pAlertLayer->showLayer("", "Ajusta donde entra la aplicación\n cuando abre utilizando\n la opción de jugada rápida");
        }
    }
    
    checkTotalPoint();
    
    AppService::hideCircleProgressbar();
    
    scheduleUpdate();
    
    playMusic();
    
    if (tmp_prev_isFB)
    {
        btn_fb->setVisible(false);
        this->schedule(schedule_selector(HelloWorld::updateFacebookLogin), 3.0);
    }
    
    //Advertisment
    if (tmp_isShowedAds == false && getShowedAds() == false)
    {
        pAdsLayer = CSLoader::createNode("Advertisment.csb");
        pAdsLayer->setVisible(false);
        pAdsLayer->setPosition(1024, 768);
        auto tmp_pnl = pAdsLayer->getChildByName("panel_ads");
        Button* pBtn_Close_ads = (Button*)tmp_pnl->getChildByName("btn_ads_close");
        pBtn_Close_ads->addTouchEventListener([=](Ref* pSender, cocos2d::ui::Widget::TouchEventType type){
            if (type == cocos2d::ui::Widget::TouchEventType::ENDED)
            {
                pAdsLayer->setVisible(false);
            }
        });
        this->addChild(pAdsLayer, 9999);
        doAdvertisment();
    }
    
    
    
    return true;
}

void HelloWorld::doAdvertisment(){
    auto url = "http://52.38.250.23:8080/ads/ads.png";
    auto request = new cocos2d::network::HttpRequest();
    request->setUrl(url);
    request->setRequestType(cocos2d::network::HttpRequest::Type::GET);
    
    request->setResponseCallback([this, url](cocos2d::network::HttpClient *, cocos2d::network::HttpResponse *response) {
        if (!response->isSucceed()) {return;}
        auto responseData = response->getResponseData();
        
        std::vector<char> *buffer = response->getResponseData();
        //const char *file_char = buffer->data();
        if (buffer->size() == 0)
            return ;
        
        if (pAdsLayer == NULL)
            return;
        
        auto tmp_pnl = pAdsLayer->getChildByName("panel_ads");        
        
        if (tmp_pnl == NULL)
            return ;
        
        
        auto dlg = tmp_pnl->getChildByName("pnl_dlg");
        
        pAdsImageView = (ImageView*)dlg->getChildByName("img_ads");
        
        Image *image = new Image();
        image->initWithImageData ( reinterpret_cast<const unsigned char*>(&(buffer->front())), buffer->size());
        
        Texture2D *texture = new Texture2D();
        texture->initWithImage(image);
        
        Sprite* spr = (Sprite*)pAdsImageView->getChildByName("spr_ads");
        spr->setTexture(texture);
        Sprite* sprAds = Sprite::createWithTexture(texture);
        sprAds->setPosition(spr->getPosition());
        
        float width = sprAds->getContentSize().width;
        float height = sprAds->getContentSize().height;
        float high_width = pAdsImageView->getContentSize().width;
        float high_height = pAdsImageView->getContentSize().height;
        sprAds->setScaleX(high_width / width);
        sprAds->setScaleY(high_height / height);
        pAdsImageView->addChild(sprAds, 9999);
        pAdsLayer->setVisible(true);
        tmp_isShowedAds = true;
        setShowedAds(true);
    });
    cocos2d::network::HttpClient::getInstance()->send(request);
}

void HelloWorld::updateFacebookLogin(float dt)
{
    this->autoFacebookLogin();
    this->unschedule(schedule_selector(HelloWorld::updateFacebookLogin));
}
void HelloWorld::autoFacebookLogin()
{
    loginFacebook();
}

void HelloWorld::updatePushMark()
{
    int newGift = 0, newMsg = 0;
    
    int cntGift = g_vGifts.size();
    if (cntGift > 0) {
        
        newGift = g_vGifts[cntGift-1].index - g_mainUserInfo.last_gift;
    }
    
    int cntMsg = g_vMessages.size();
    if (cntMsg > 0) {
        
        newMsg = g_vMessages[cntMsg-1].index - g_mainUserInfo.last_msg;
    }
    
    int newItem = newGift+newMsg;
    if (newItem > 0) {
        
        img_push->setVisible(true);
    }
    else {
        
        img_push->setVisible(false);
    }
}

void HelloWorld::updateString2Lang()
{
    for (int i = 0; i < 3; i++) {
        
       lb_title[i]->setString(g_strHelloWorld[i][g_nLangType]);
    }
    
    // img_soon->loadTexture(g_imgHelloWorld[0][g_nLangType], TextureResType::PLIST);
    lb_ribbon_1->setString(g_strHelloWorld[4][g_nLangType]);
    lb_ribbon_2->setString(g_strHelloWorld[5][g_nLangType]);
    
    panel_bonus->setBackGroundImage(g_imgHelloWorld[1][g_nLangType], TextureResType::PLIST);
    
    pCarouselLayer->updateImg2Lang();
}

void HelloWorld::actionNewFriend(std::string msg)
{
    showAlerMsg(g_strHelloWorld[3][g_nLangType], msg);
    if (pSocialLayer->isVisible() == true) {
        
        pSocialLayer->actionNewFriend();
    }
}

void HelloWorld::actionNewMsg(std::string msg)
{
    showAlerMsg(g_strHelloWorld[3][g_nLangType], msg);
    updatePushMark();
    if (pSocialLayer->isVisible() == true) {
        
        pSocialLayer->actionNewMsg();
    }
}

void HelloWorld::actionNewGift(std::string msg)
{
    showAlerMsg(g_strHelloWorld[3][g_nLangType], msg);
    updatePushMark();
    if (pSocialLayer->isVisible() == true) {
        
        pSocialLayer->actionNewGift();
    }
}

void HelloWorld::actionNewBonus(std::string msg)
{
    showAlerMsg(g_strHelloWorld[3][g_nLangType], msg);
}

void HelloWorld::actionNewFreeSpin(std::string msg)
{
    showAlerMsg(g_strHelloWorld[3][g_nLangType], msg);
}

void HelloWorld::showBonusNode()
{
//    bonus_node->stopAllActions();
//    bonus_node->setVisible(true);
//    bonus_node->runAction(pNodeAnimation);
//    pNodeAnimation->setTimeSpeed(0.3);
//    pNodeAnimation->gotoFrameAndPlay(0, true);
//    bonus_node->setVisible(true);
    
    panel_bonus->setVisible(true);
}

void HelloWorld::showProfileInfo()
{
    if (g_mainUserInfo.points < 0) {
        
        g_mainUserInfo.points = 10000;
    }
    
    lb_name->setString(g_mainUserInfo.user_name);
    lb_level->setString(StringUtils::format("Level %d", g_mainUserInfo.level+1));
    
    float percent = g_mainUserInfo.level_point/(float)g_nLevelupXP[g_mainUserInfo.level]*100;
    level_bar->setPercent(percent);
    lb_points->setString(getNormalText2Number(g_mainUserInfo.points));
    WebService::getInstance()->setProfileImage(g_mainUserInfo.photo_id, img_photo);
    
    img_levelbar->setVisible(true);
    img_photo->setVisible(true);
}

void HelloWorld::onUpdatedDone()
{
   showProfileInfo();
}

void HelloWorld::onGetFreesSpinDone()
{
    pSpinLayer->showLayer();
}

void HelloWorld::onFBConnectDone()
{
    std::string strFB_ID = cocos2d::UserDefault::getInstance()->getStringForKey(FBID_KEY);
    if (strFB_ID.compare(Facebook::getInstance()->getUserId()) == 0) {
        
        return;
    }
 
    AppService::showCircleProgressbar();
    WebService::getInstance()->login2FB(Facebook::getInstance()->getUserId());
    
    tmp_prev_isFB = false;
    
    btn_fb->setVisible(false);
    setGiftBonusFlag(false);
    panel_bonus->setVisible(false);
    img_bonustimer->setVisible(false);
    
    //g_bLoginFB = true;
    //btn_fb->setTitleText("SIGN OUT FB");
    
//    lb_name->setString(g_mainUserInfo.user_name.c_str());
//    WebService::getInstance()->setProfileImage(g_mainUserInfo.photo_id, img_photo);
    
    //showFBInviteLayer();
    //fb image download
    //loadFBProfileImg(g_mainUserInfo.fb_id);
}

//void HelloWorld::loadFBProfileImg(std::string user_id)
//{
//    char url[1024] = "";
//    //sprintf(url, "https://graph.facebook.com/%s/picture?type=small", user_id.c_str());
//    sprintf(url, "https://graph.facebook.com/%s/picture?height=128&width=128", user_id.c_str());
//    
//    HttpRequest* request = new HttpRequest();
//    request->setRequestType(HttpRequest::Type::GET);
//    request->setUrl(url);
//    request->setResponseCallback(CC_CALLBACK_2(HelloWorld::onImgDownloadCompleted, this));
//    request->setTag("Get Profile Image");
//    //HttpClient::getInstance()->send(request);
//    HttpClient::getInstance()->sendImmediate(request);
//    
//    request->release();
//    
//    //WebService::getInstance()->login();
//}
//
//void HelloWorld::onImgDownloadCompleted(HttpClient* sender, HttpResponse* response)
//{
//    if (response->isSucceed() == false) return;
//    
//    std::string path = FileUtils::getInstance()->getWritablePath();
//    std::string file_name = g_mainUserInfo.fb_id + ".jpg";
//    std::string file_path = path+file_name;
//    
//    FILE* fp = fopen(file_path.c_str(), "w+b");
//    
//    if (fp == NULL)
//        return;
//    
//    std::vector<char> *buffer = response->getResponseData();
//    unsigned char bf;
//    for (unsigned int i = 0; i < buffer->size(); i++) {
//        
//        bf = buffer->at(i);
//        fwrite(&bf, 1, 1, fp);
//    }
//    fclose(fp);
//  
//    img_photo->loadTexture(file_name);
//}

void HelloWorld::showFBBonusLayer()
{
    pFBBonusLayer->showLayer();
}

void HelloWorld::showFBInviteLayer()
{
    pFBInviteLayer->showLayer();
}

void HelloWorld::onKeyReleased(EventKeyboard::KeyCode keyCode, Event *event)
{
    if(keyCode == EventKeyboard::KeyCode::KEY_BACK) {
        
        Director::getInstance()->end();
        
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        exit(0);
#endif
    }
}

void HelloWorld::loginFacebook()
{
    if(pPrivateProfileLayer->isVisible() == true) {
    
        pPrivateProfileLayer->hideLayer();
    }
    
    g_nFBLoginType = 0;
    Facebook::getInstance()->login();
}

void HelloWorld::logoutFacebook()
{
    pPrivateProfileLayer->hideLayer();

    Facebook::getInstance()->logout();

}

void HelloWorld::onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
    if (type == Widget::TouchEventType::ENDED) {
        
        if(btn_id == 0) { //store
            
            pStoreLayer->showLayer();
        }
        else if(btn_id == 1) { //fb
            
            pFBConnectLayer->showLayer();
        }
        else if(btn_id == 2) { //social
            
            pSocialLayer->showLayer();
        }
        else if(btn_id == 3) { //help
            
            Scene *pScene = TransitionFade::create(0.5, TutorialScene::createScene());
            Director::getInstance()->replaceScene(pScene);
        }
        else if(btn_id == 4) { //setting
            
            pSettingLayer->showLayer();
        }
        else if(btn_id == 5) { //tournament
            
        }
        else if(btn_id == 6) { //leaderboard
            
            pLeaderboardLayer->showLayer();
        }
        else if(btn_id == 7) { //roulette
            
            pSpinLayer->showLayer();
        }
        else if(btn_id == 8) { //bonus
            
            panel_bonus->setVisible(false);
            setGiftBonusFlag(false);
            pGiftBonusLayer->showLayer();
        }
        else if(btn_id == 9) { //photo
            
            pPrivateProfileLayer->showLayer();
        }
        
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
    }
}

void HelloWorld::playMusic()
{
    bool flag = CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying();
    if(flag == true) return;
    
    if (g_bMusic == true) {
        
        //CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
        CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("lobby.aac", true);
    }
}

void HelloWorld::update(float delta)
{
    if (g_mainUserInfo.points > 19)
    {
        setLessGiftFlag(false);
        setGiftBonusFlag(false);
        img_bonustimer->setVisible(false);
        return ;
    }
    
    if (getLessGiftFlag() == false)
        return ;
    
    time_t stTime = getGiftBonusTime();
    if (stTime == 0) {
        return;
    }
    
    time_t cur_time;
    time(&cur_time);
    double seconds = difftime(cur_time, stTime);
    
    int time_limit = 180*60;
    if (seconds > time_limit) {
        
        img_bonustimer->setVisible(false);

        setLessGiftFlag(true);
        setGiftBonusFlag(true);
        showBonusNode();
        
    }
    else {
        
        img_bonustimer->setVisible(true);

        long duration = time_limit - seconds;
        //int hour = duration / 60 / 60;
        int minute = (duration / 60); //% 60;
        int second = duration % 60;
        
        lb_bonustime->setString(StringUtils::format("%02d:%02d", minute, second));
    }
}
