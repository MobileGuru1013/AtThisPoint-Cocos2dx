#include "HelloWorldScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

#include "GameScene_Online.h"
#include "Table.h"
#include "GameController.h"
#include "WebService.h"

#include "AppService.h"

using namespace cocostudio::timeline;

//define string
std::string g_strGameScene[7][2] = {
    {"Click here for chat with others", "Haz click para chatear con otros"},
    {"You:", "Tu:"},
    {"FOLD", "PASAR"},
    {"BET", "ARRIESGAR"},
    {"Opps! You're out of points!", "Uyy! Te has quedado sin puntos!"},
    {"Or\nWait 3 hours for free points", "O\nEspera 3 horas por puntos gratis"},
    {"Buy Now", "Compra Ahora"}
};

std::string g_imgGameScene[3][2] = {{"table_user.png", "_table_user.png"},
                        {"text_win.png", "_text_win.png"},
                        {"text_loose.png", "_text_loose.png"}
};

Scene* GameScene_Online::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = GameScene_Online::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool GameScene_Online::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}
    
    CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
    CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    g_pHelloWorld = NULL;
    
    exitFlag = false;
    isEnablePlay = false;
    
    //init parameters for game playing
    playtime = 0;
    win_streak = 0;
    flagPlaying = false;

	//touch enable
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	auto listener = EventListenerTouchAllAtOnce::create();
	listener->onTouchesBegan = CC_CALLBACK_2(GameScene_Online::onTouchesBegan, this);
	listener->onTouchesEnded = CC_CALLBACK_2(GameScene_Online::onTouchesEnded, this);
	listener->onTouchesMoved = CC_CALLBACK_2(GameScene_Online::onTouchesMoved, this);
	dispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	
	auto rootNode = CSLoader::createNode("TableScene.csb");
	addChild(rootNode);
    
    //monkey node
    node_monkey = (Node*)rootNode->getChildByName("monkeyNode");
    pMonkeyAnimation = CSLoader::createTimeline("MonkeyNode.csb");
    pMonkeyAnimation->retain();
    pMonkeyAnimation->gotoFrameAndPause(0);
    
    auto panel_check = (Layout*)rootNode->getChildByName("panel_deck");
    panel_check->addTouchEventListener(CC_CALLBACK_2(GameScene_Online::onMainBtnClicked, this, 6));
    
    img_table = (ImageView*)rootNode->getChildByName("table");

	//player panel
	for (int i = 0; i < MAX_PLAYERS; i++) {

		auto player_panel = static_cast<Layout*>(rootNode->getChildByName(StringUtils::format("panel_%d", i)));
        player_panel->setScaleX(g_fScaleWH);
        player_panel->setVisible(false);
        panelProfile[i] = player_panel;
        
        img_gift[i] = (ImageView*)player_panel->getChildByName("gift");
        img_gift[i]->setVisible(false);
        
        panelInfo[i] = static_cast<Layout*>(player_panel->getChildByName("main"));
        panelInfo[i]->addTouchEventListener(CC_CALLBACK_2(GameScene_Online::onProfileClicked, this, i));
        panelInfo[i]->setLocalZOrder(3);
        img_active[i] = static_cast<ImageView*>(panelInfo[i]->getChildByName("active"));
        //img_active[i]->setLocalZOrder(5);
        img_active[i]->setVisible(false);
        //auto img_level = static_cast<ImageView*>(panelInfo[i]->getChildByName("level_mark"));
        //img_level->setLocalZOrder(1);
        img_dealer[i] = static_cast<ImageView*>(panelInfo[i]->getChildByName("dealer"));
        //img_dealer[i]->setLocalZOrder(1);
        img_dealer[i]->setVisible(false);
        img_levelbg[i] = static_cast<ImageView*>(panelInfo[i]->getChildByName("level_mark"));
        img_levelbg[i]->setVisible(false);
        
		lb_name[i] = static_cast<Text*>(panelInfo[i]->getChildByName("name"));
        //lb_name[i]->setLocalZOrder(1);
        lb_name[i]->setString("");
        lb_level[i] = static_cast<Text*>(panelInfo[i]->getChildByName("level"));
        //lb_level[i]->setLocalZOrder(2);
        lb_level[i]->setString("");
        lb_point[i] = static_cast<Text*>(panelInfo[i]->getChildByName("point"));
        //lb_point[i]->setLocalZOrder(1);
        lb_point[i]->setString("");

        img_ebg[i] = static_cast<ImageView*>(panelInfo[i]->getChildByName("e_bg"));
        img_ebg[i]->setVisible(false);
        
        img_emoticon[i] = static_cast<ImageView*>(panelInfo[i]->getChildByName("emoticon"));
        img_emoticon[i]->setVisible(false);
       
        img_invite[i] = static_cast<ImageView*>(player_panel->getChildByName("bg"));
        img_invite[i]->loadTexture(g_imgGameScene[0][g_nLangType], TextureResType::PLIST);
        img_invite[i]->setVisible(false);
        img_invite[i]->addTouchEventListener(CC_CALLBACK_2(::GameScene_Online::onInviteBtnClicked, this, i));
        
        //clipping node
        auto clippingNode = ClippingNode::create();
        clippingNode->setAlphaThreshold(MASKING_THRESHOLD);
        clippingNode->setPosition(0, 0);
        clippingNode->setAnchorPoint(Vec2(0, 0));
        rootNode->addChild(clippingNode);
        //clippingNode->setLocalZOrder(0);

        auto mask = Sprite::createWithSpriteFrameName("img_mask.png");
        float scaleX = (player_panel->getBoundingBox().size.width) / (float)mask->getBoundingBox().size.width;
        float scaleY = (player_panel->getBoundingBox().size.height) / (float)mask->getBoundingBox().size.height;
        mask->setScale(scaleX, scaleY);
        mask->setPosition(player_panel->getPosition());
        clippingNode->setStencil(mask);

        img_photo[i] = ImageView::create("img_mask.png", TextureResType::PLIST);
        float scaleX1 = (player_panel->getBoundingBox().size.width-10) / (float)img_photo[i]->getBoundingBox().size.width;
        float scaleY1 = (player_panel->getBoundingBox().size.height-10) / (float)img_photo[i]->getBoundingBox().size.height;
        img_photo[i]->setVisible(false);
        img_photo[i]->setScale(scaleX1, scaleY1);
        img_photo[i]->setPosition(player_panel->getPosition());
        clippingNode->addChild(img_photo[i]);
        
        auto imgBorder = ImageView::create("border.png", TextureResType::PLIST);
        imgBorder->setVisible(false);
        imgBorder->setScale(scaleX, scaleY);
        imgBorder->setPosition(player_panel->getPosition());
        rootNode->addChild(imgBorder);
        
        img_border[i] = imgBorder;
        
        clippingNode->setLocalZOrder(0);
        player_panel->setLocalZOrder(3);
    }

    //show cards
    for (int i = 0; i < 2; i++) {
        
        auto img_showcard = static_cast<ImageView*>(rootNode->getChildByName(StringUtils::format("pcard_%d", i)));
        posPCards[i] = img_showcard->getPosition();
        if(i == 1) {
            
            img_pcard = img_showcard;
            img_pcard->setPosition(posPCards[0]);
        }
        
        img_showcard = static_cast<ImageView*>(rootNode->getChildByName(StringUtils::format("gcard_%d", i)));
        posGCards[i] = img_showcard->getPosition();
        if(i == 1) {
            
            img_gcard = img_showcard;
            img_gcard->setPosition(posGCards[0]);
        }
    }
    
	//hands image
	for (int i = 0; i < MAX_PLAYERS; i++) {

		img_hands[i] = static_cast<ImageView*>(rootNode->getChildByName(StringUtils::format("handcard_%d", i)));
		img_hands[i]->setVisible(false);
        
        img_chips[i] = static_cast<ImageView*>(rootNode->getChildByName(StringUtils::format("chip_%d", i)));
        img_chips[i]->setVisible(false);
        lb_chips[i] = static_cast<Text*>(img_chips[i]->getChildByName("point"));
        
        img_chat[i] = (ImageView*)rootNode->getChildByName(StringUtils::format("bubble_%d", i));
        //img_chat[i]->ignoreContentAdaptWithSize(false);
        img_chat[i]->setVisible(false);
        lb_chat[i] = (Text*)img_chat[i]->getChildByName("text");
        lb_chat[i]->ignoreContentAdaptWithSize(true);
        img_chat[i]->setLocalZOrder(4);
        
        img_chat2[i] = (ImageView*)rootNode->getChildByName(StringUtils::format("bubble2_%d", i));
        //img_chat2[i]->ignoreContentAdaptWithSize(false);
        img_chat2[i]->setVisible(false);
        img_chat2[i]->setLocalZOrder(4);
        
        lb_chat2[i][0] = (Text*)img_chat2[i]->getChildByName("text_0");
        lb_chat2[i][0]->ignoreContentAdaptWithSize(true);
        lb_chat2[i][1] = (Text*)img_chat2[i]->getChildByName("text_1");
        lb_chat2[i][1]->ignoreContentAdaptWithSize(true);
    }

	//deck image
	img_deck = static_cast<ImageView*>(rootNode->getChildByName("deck"));
	img_deck->setVisible(true);
    
    //back cards
    for (int i = 0; i < 40; i++) {
        
        img_backCards[i] = static_cast<ImageView*>(rootNode->getChildByName(StringUtils::format("deck_%d", i)));
        img_backCards[i]->setVisible(true);
    }
    
	//pot
	img_pot = static_cast<ImageView*>(rootNode->getChildByName("pot"));
	img_pot->setVisible(false);
    lb_pot = static_cast<Text*>(img_pot->getChildByName("point"));
	
	for (int i = 0; i < 4; i++) {

		std::string strName = StringUtils::format("btn_%d", i);
		auto btnMain = static_cast<Button*>(rootNode->getChildByName(strName));
		btnMain->addTouchEventListener(CC_CALLBACK_2(GameScene_Online::onMainBtnClicked, this, i));
        
        btnMain->setScaleX(g_fScaleWH);
        if(i == 1) {
            
            btn_sound = btnMain;
            if(g_bSound == false) {
                
                btn_sound->loadTextures("btn_soundoff.png", "btn_soundoff_.png", "btn_soundoff_.png", TextureResType::PLIST);
            }
            else {
                
                btn_sound->loadTextures("btn_sound.png", "btn_sound_.png", "btn_sound_.png", TextureResType::PLIST);
            }
        }
        else if(i == 3) {
            
            btn_chat = btnMain;
            lb_title[0] = (Text*)btn_chat->getChildByName("text");
            lv_chathistory = (ListView*)btn_chat->getChildByName("lv_history");
            lv_chathistory->setItemsMargin(3);
            lv_chathistory->setDefaultAlign(false);
        }
	}
    
    //init win and lose mark
    img_winmark = (ImageView*)rootNode->getChildByName("img_winner");
    img_winmark->setScaleX(g_fScaleWH);
    img_winmark->setLocalZOrder(3);
    img_winmark->setVisible(false);
    
    auto img_wintext = (ImageView*)img_winmark->getChildByName("label_mark");
    img_wintext->loadTexture(g_imgGameScene[1][g_nLangType], TextureResType::PLIST);
    
    img_losemark = (ImageView*)rootNode->getChildByName("img_looser");
    img_losemark->setScaleX(g_fScaleWH);
    img_losemark->setLocalZOrder(3);
    img_losemark->setVisible(false);
    
    auto img_loosetext = (ImageView*)img_losemark->getChildByName("label_mark");
    img_loosetext->loadTexture(g_imgGameScene[2][g_nLangType], TextureResType::PLIST);
    
    //action panel
    panelAction = static_cast<Layout*>(rootNode->getChildByName("action_panel"));
    panelAction->setScaleX(g_fScaleWH);
    panelAction->setVisible(false);
    for (int i = 0; i < 2; i++) {
        
        std::string strName = StringUtils::format("btn_%d", i);
        auto btnMain = static_cast<Button*>(panelAction->getChildByName(strName));
        btnMain->addTouchEventListener(CC_CALLBACK_2(GameScene_Online::onMainBtnClicked, this, i+4));
        
        auto lb_text = (Text*)btnMain->getChildByName("text");
        lb_text->setString(g_strGameScene[i+2][g_nLangType]);
   }
    
   //betting panel
   panelBetting = static_cast<Layout*>(rootNode->getChildByName("betting_panel"));
   panelBetting->setLocalZOrder(50);
   panelBetting->setVisible(false);
   slider_betting = static_cast<Slider*>(panelBetting->getChildByName("slider"));
   slider_betting->addEventListener(CC_CALLBACK_2(GameScene_Online::onSliderEvent, this));
   auto imgBubble = static_cast<ImageView*>(panelBetting->getChildByName("bubble"));
   lb_betting = (Text*)(imgBubble->getChildByName("lb_value"));
   cb_betting = (Button*)(panelBetting->getChildByName("check"));
   cb_betting->addTouchEventListener(CC_CALLBACK_2(GameScene_Online::onBettingChecked, this, 0));
   auto btn_maxbet = (Button*)(panelBetting->getChildByName("max_bet"));
   btn_maxbet->addTouchEventListener(CC_CALLBACK_2(GameScene_Online::onBettingChecked, this, 1));
    
   //particles
   auto img_tablelights = (ImageView*)rootNode->getChildByName("lights_panel");
   for(int i = 0; i < 23; i++) {
        
        particle_lights[i] = static_cast<ParticleSystem*>(img_tablelights->getChildByName(StringUtils::format("particle_%d", i)));
       
        particle_lights[i]->stopSystem();
   }
   for (int i = 0; i < 48; i++) {
        
       img_lights[i] = (ImageView*)img_tablelights->getChildByName(StringUtils::format("light_%d", i));
   }
    
   particle_cleanpot = static_cast<ParticleSystem*>(rootNode->getChildByName("particle_cleanpot"));
   particle_cleanpot->setLocalZOrder(40);
   particle_cleanpot->stopSystem();
   for(int i = 0; i < 4; i++) {
   
       particle_chip[i] = static_cast<ParticleSystem*>(rootNode->getChildByName(StringUtils::format("particle_chip%d", i)));
       particle_chip[i]->setLocalZOrder(40);
       particle_chip[i]->stopSystem();
   }
   for(int i = 0; i < 4; i++) {
        
       particle_win[i] = static_cast<ParticleSystem*>(rootNode->getChildByName(StringUtils::format("particle_win%d", i)));
       particle_win[i]->setLocalZOrder(40);
       particle_win[i]->stopSystem();
   }
   for (int i = 0; i < 2; i++) {
        
      particle_lose[i] = static_cast<ParticleSystem*>(rootNode->getChildByName(StringUtils::format("particle_lose%d", i)));
      particle_lose[i]->setLocalZOrder(40);
      particle_lose[i]->stopSystem();
   }
    //init find team layer
//    strName = "FindTeamLayer.csb";
//    if (MAX_PLAYERS == 2) {
//        
//        strName = "FindTeamLayer_2.csb";
//    }
//    layerFindTeam = CSLoader::createNode(strName);
//    layerFindTeam->setVisible(false);
//    addChild(layerFindTeam, 100);
//    
//    auto btnClose = static_cast<Button*>(layerFindTeam->getChildByName("btn_0"));
//    btnClose->setScaleX(g_fScaleWH);
//    btnClose->addTouchEventListener(CC_CALLBACK_2(GameScene_Online::onFindTeamBtnClicked, this, 0));
//    
//    panelStart = static_cast<Layout*>(layerFindTeam->getChildByName("panel_1"));
//    panelStart->setVisible(false);
//    auto btnStart = static_cast<Button*>(panelStart->getChildByName("btn_start"));
//    btnStart->addTouchEventListener(CC_CALLBACK_2(GameScene_Online::onFindTeamBtnClicked, this, 1));
//
//    auto panelTeam = static_cast<Layout*>(layerFindTeam->getChildByName("panel_0"));
	//card images
//	for (int i = 0; i < NUM_CARDS; i++) {
//
//		spriteCards[i] = Sprite::createWithSpriteFrameName("card_back.png");
//		spriteCards[i]->setVisible(false);
//		//spriteCards_front[i]->setScale(getScaleValX()*1.5*scaleX_Card, getScaleValY()*1.5*scaleY_Card);
//		this->addChild(spriteCards[i], 28);
//        
//        Size szCard =spriteCards[i]->getBoundingBox().size;
//        spriteCards_d[i] = Sprite::createWithSpriteFrameName("card_.png");
//        spriteCards_d[i]->setVisible(false);
//        spriteCards_d[i]->setPosition(Vec2(szCard.width*0.5, szCard.height*0.5));
//        spriteCards[i]->addChild(spriteCards_d[i]);
//        
//        spriteCards_back[i] = Sprite::createWithSpriteFrameName("card_back.png");
//        spriteCards_back[i]->setVisible(false);
//        //spriteCards_front[i]->setScale(getScaleValX()*1.5*scaleX_Card, getScaleValY()*1.5*scaleY_Card);
//        this->addChild(spriteCards_back[i], 28);
//	}

    //init Waiting layer
//    pWaitingLayer = new WaitingLayer();
//    pWaitingLayer->createLayer(this, 100, this);
//    
//    //init Friend layer
//    pFriendLayer = new FriendLayer();
//    pFriendLayer->createLayer(this, 100, this);
//    
//    //init Setting layer
//    pSettingLayer = new SettingLayer();
//    pSettingLayer->createLayer(this, 100, this);
    
	//init alert layer
	pAlertLayer = new AlertLayer();
	pAlertLayer->createLayer(this, 100, this);
    
    //init chat layer
    pChatLayer = new ChatLayer();
    pChatLayer->createLayer(this, 100, this);
    
    //init profile layer
    pPublicProfileLayer = new PublicProfileLayer();
    pPublicProfileLayer->createLayer(this, 40, this);
    
    //init levelup layer
    pLevelupLayer = new LevelupLayer();
    pLevelupLayer->createLayer(this, 90, this);
    
    //init store layer
    pStoreLayer = new StoreLayer();
    pStoreLayer->createLayer(this, 95, this);
    
    g_pStoreLayer = pStoreLayer;
    
    //init spin layer
    pSpinLayer = new SpinLayer();
    pSpinLayer->createLayer(this, 120, this);
    
    g_pSpinLayer = pSpinLayer;
    
    //init table invite layer
    pTableInviteLayer = new TableInviteLayer();
    pTableInviteLayer->createLayer(this, 120, this);
    
    //init connecting layer
    pConnectingLayer = new ConnectingLayer();
    pConnectingLayer->createLayer(this, 200, this);
    
//	//init effect layer
//	pEffectLayer = new EffectLayer();
//	pEffectLayer->createLayer(this, 500);
//    
//    //init round alarm layer
//    pRoundAlarmLayer = new RoundAlarmLayer();
//    pRoundAlarmLayer->createLayer(this, 500);
//
//	//init result layer
//	pResultLayer = new ResultLayer();
//	pResultLayer->createLayer(this, 300, this);
    
    //init chips, gifts for animation
    for (int i = 0; i < MAX_PLAYERS; i++) {
        
        auto chipNode = CSLoader::createNode("ChipNode.csb");
        this->addChild(chipNode);
        imgAnimationChips[i] = static_cast<ImageView*>(chipNode->getChildByName("chip"));
        imgAnimationChips[i]->setScaleX(g_fScaleWH);
        imgAnimationChips[i]->setVisible(false);
        lb_AnimationChips[i] = static_cast<Text*>(imgAnimationChips[i]->getChildByName("point"));
        
        imgAnimationGifts[i] = ImageView::create("gifticon_0.png", TextureResType::PLIST);
        imgAnimationGifts[i]->setVisible(false);
        this->addChild(imgAnimationGifts[i], 5);
    }
    
    //init countdown timer
    initTimerCountdown(rootNode);

    for (int i = 0; i < MAX_PLAYERS; i++) {
        
        gamePlayers[MAIN_USER].init();
        //panelInfo[i]->setVisible(false);
    }

    //init main player info
    lb_name[MAIN_USER]->setString(g_mainUserInfo.user_name);
    WebService::getInstance()->setProfileImage(g_mainUserInfo.photo_id, img_photo[MAIN_USER]);
    
    gamePlayers[MAIN_USER].info = g_mainUserInfo;
    
    gamePlayers[MAIN_USER].flagEmpty = false;
    gamePlayers[MAIN_USER].user_id = g_mainUserInfo.user_id;
    gamePlayers[MAIN_USER].name = g_mainUserInfo.user_name;
    gamePlayers[MAIN_USER].photo_id = g_mainUserInfo.photo_id;
    gamePlayers[MAIN_USER].level = g_mainUserInfo.level;
    gamePlayers[MAIN_USER].points = g_nBuyin;
    showPlayer(MAIN_USER);
    
    if (g_nRoomType == MASTER_ROOM) {
        
        for (int i = 1; i < MAX_PLAYERS; i++) {
            
            img_invite[i]->setVisible(true);
        }
    }
    
    //Less Point Layer
    importLessLayer();
    
    
    
    //
    
    timer_index = INVALID;
    durationTimer = 0;

    numRounds = 0;
    gameStatus = STOP_STATUS;
    
    GameController::getInstance()->setViewListner(this);
    GameController::getInstance()->connect2AppWarp();
	//startGame();
    
//    if (g_nRoomType != MASTER_ROOM) {
//        
//        pWaitingLayer->showLayer();
//    }
//    else {
//        server_position = 0;
//        player_num = 1;
//        //show main user info, first
//        WebService::getInstance()->setProfileImage(g_mainUserInfo.photo_id, img_teamPhoto[0]);
//        lb_teamName[0]->setString(g_mainUserInfo.user_name);
//    }
    
    updateString2Lang();
    
    scheduleUpdate();

	return true;
}

void GameScene_Online::importLessLayer()
{
    pLessPointLayer = (Node*)CSLoader::createNode("LessPointPopup.csb");
    this->addChild(pLessPointLayer, 9999);
    pLessPointLayer->setVisible(false);
    Button* btnClose = (Button*)((Sprite*)pLessPointLayer->getChildByName("bg"))->getChildByName("btn_close");
    btnClose->addTouchEventListener([=](Ref* pSender, cocos2d::ui::Widget::TouchEventType type) {
        if (type == cocos2d::ui::Widget::TouchEventType::ENDED)
        {
            pLessPointLayer->setVisible(false);
            // exitGame();
        }
    });
    
    Button* btnBuyNow = (Button*)((Sprite*)pLessPointLayer->getChildByName("bg"))->getChildByName("btn_buy");
    btnBuyNow->addTouchEventListener([=](Ref* pSender, cocos2d::ui::Widget::TouchEventType type) {
        if (type == cocos2d::ui::Widget::TouchEventType::ENDED)
        {
            pLessPointLayer->setVisible(false);
            pStoreLayer->showLayer();
        }
    });
}

void GameScene_Online::showLessLayer()
{
    pLessPointLayer->setVisible(true);
    Text* lbl_title = (Text*)((Sprite*)pLessPointLayer->getChildByName("bg"))->getChildByName("lbl_title");
    Text* lbl_text = (Text*)((Sprite*)pLessPointLayer->getChildByName("bg"))->getChildByName("lbl_text");
    Button* btnBuyNow = (Button*)((Sprite*)pLessPointLayer->getChildByName("bg"))->getChildByName("btn_buy");
    lbl_title->setString(g_strGameScene[4][g_nLangType]);
    lbl_text->setString(g_strGameScene[5][g_nLangType]);
    btnBuyNow->setTitleText(g_strGameScene[6][g_nLangType]);
    
    if (getLessGiftFlag() == false)
    {
        setLessGiftFlag(true);
        checkTotalPoint();
    }
}

void GameScene_Online::updateString2Lang()
{
    for (int i = 0; i < 1; i++) {
        
        lb_title[i]->setString(g_strGameScene[i][g_nLangType]);
    }
}

void GameScene_Online::sendEmoticonItem(int sender_id, int receiver_id, int item_id)
{
    if (sender_id == -1 ||  receiver_id == -1 || item_id == -1) {
        
        return;
    }
    
    int avatar_index = item_id/12;
    int index = item_id%12;
    std::string strItemName = StringUtils::format("emoticon_%d_%d.png", avatar_index, index);
    
    int bg_index = rand()%12;
    std::string strBg = StringUtils::format("emoticon_bg%d.png", bg_index);
    
    gamePlayers[receiver_id].emoticon_id = item_id;
    gamePlayers[receiver_id].emoticon_timer = 0;
    
    if(sender_id == receiver_id) { //for oneself
        
        img_ebg[receiver_id]->loadTexture(strBg, TextureResType::PLIST);
        img_ebg[receiver_id]->setVisible(true);
        
        img_emoticon[receiver_id]->loadTexture(strItemName, TextureResType::PLIST);
        img_emoticon[receiver_id]->setVisible(true);
    }
    else { //for other players
        
        Vec2 src_pos = panelProfile[sender_id]->getPosition();
        
        imgAnimationGifts[receiver_id]->loadTexture(strItemName, TextureResType::PLIST);
        imgAnimationGifts[receiver_id]->setPosition(src_pos);
        imgAnimationGifts[receiver_id]->setScale(0.1);
        imgAnimationGifts[receiver_id]->setVisible(true);
        
        Vec2 tar_pos = panelProfile[receiver_id]->getPosition();
        //tar_pos.x += img_gift[receiver_id]->getPosition().x;
        //tar_pos.y += img_gift[receiver_id]->getPosition().y;
        imgAnimationGifts[receiver_id]->runAction(Sequence::create(
                                                                   DelayTime::create(0.1),
                                                                   Spawn::create(                                                                                                                                                                              MoveTo::create(1, tar_pos),
                                                                                 ScaleTo::create(1, 0.7), NULL
                                                                                 ),
                                                                   CallFunc::create([=]()->void{
                                                                        imgAnimationGifts[receiver_id]->setScale(1);
                                                                        imgAnimationGifts[receiver_id]->setVisible(false);
            
                                                                        img_ebg[receiver_id]->loadTexture(strBg, TextureResType::PLIST);
                                                                        img_ebg[receiver_id]->setVisible(true);
                                                                        img_emoticon[receiver_id]->loadTexture(strItemName, TextureResType::PLIST);
                                                                        img_emoticon[receiver_id]->setVisible(true);
                                                                    }),
                                                                   NULL));
        //                                                        CallFunc::create(CC_CALLBACK_0(HelloWorld::coinIncreasing, this, k)),
        //                                                        NULL));
    }
}

void GameScene_Online::sendGiftItem(int sender_id, int receiver_id, int item_id)
{
    if (sender_id == -1 ||  receiver_id == -1 || item_id == -1) {
        
        return;
    }
    
    int index = item_id; //getPresentItemIndex(item_id);
    std::string strItemName = StringUtils::format("gifticon_%d.png", item_id); //g_vPresnetList[index].item_photo;
    
    gamePlayers[receiver_id].gift_id = item_id;
//    if(receiver_id == MAIN_PLAYER) {
//        
//        g_mainProfileInfo.present_id = item_id;
//    }
    
    if(sender_id == receiver_id) { //for oneself
        
        img_gift[receiver_id]->loadTexture(strItemName, TextureResType::PLIST);
        img_gift[receiver_id]->setVisible(true);
    }
    else { //for other players
        
        Vec2 src_pos = panelProfile[sender_id]->getPosition();
        
        imgAnimationGifts[receiver_id]->loadTexture(strItemName, TextureResType::PLIST);
        imgAnimationGifts[receiver_id]->setPosition(src_pos);
        imgAnimationGifts[receiver_id]->setScale(0.1);
        imgAnimationGifts[receiver_id]->setVisible(true);
        
        Vec2 tar_pos = panelProfile[receiver_id]->getPosition();
        //tar_pos.x += img_gift[receiver_id]->getPosition().x;
        //tar_pos.y += img_gift[receiver_id]->getPosition().y;
        imgAnimationGifts[receiver_id]->runAction(Sequence::create(
                                                                      DelayTime::create(0.1),
                                                                      Spawn::create(                                                                                                                                                                              MoveTo::create(1, tar_pos),
                                                                                    ScaleTo::create(1, 0.39), NULL
                                                                                    ),
                                                                      CallFunc::create([=]()->void{
            imgAnimationGifts[receiver_id]->setScale(1);
            imgAnimationGifts[receiver_id]->setVisible(false);
            img_gift[receiver_id]->loadTexture(strItemName, TextureResType::PLIST);
            img_gift[receiver_id]->setVisible(true);
        }),
                                                                      NULL));
        //                                                        CallFunc::create(CC_CALLBACK_0(HelloWorld::coinIncreasing, this, k)),
        //                                                        NULL));
    }
}

void GameScene_Online::cleanup()
{
	unscheduleAllCallbacks();

	this->removeAllChildrenWithCleanup(true);

	//Director::getInstance()->purgeCachedData();
}

void GameScene_Online::onGetFreesSpinDone()
{
    pSpinLayer->showLayer();
}

void GameScene_Online::onUpdatedDone()
{
    GameController::getInstance()->sendUserInfo();
    
    if (child_index == STORE_LAYER) {
        
        WebService::getInstance()->setProfileImage(g_mainUserInfo.photo_id, img_photo[MAIN_USER]);
    }
}

void GameScene_Online::showAlert(std::string msg)
{
	pAlertLayer->showLayer("", msg);
//
//	DelayTime *delayTime = CCDelayTime::create(1);
//	CallFunc *call_func = CallFunc::create(CC_CALLBACK_0(AlertLayer::hideLayer, pAlertLayer));
//	Sequence *delayAct = Sequence::create(delayTime, call_func, NULL);
//	this->runAction(delayAct);
}

void GameScene_Online::onInviteBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
    if (type == Widget::TouchEventType::ENDED) {
//        
//        pFriendLayer->setInvitePos(btn_id);
//        pFriendLayer->showLayer(1);
    }
}

void GameScene_Online::onFindTeamBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
    if (type == Widget::TouchEventType::ENDED) {
        
        //layerFindTeam->setVisible(false);
    }
}

void GameScene_Online::onSliderEvent(Ref* sender, Slider::EventType type)
{
    if (type == Slider::EventType::ON_PERCENTAGE_CHANGED) {
        
        float percent = 0.01f*slider_betting->getPercent();
        cur_betting = (int)(min_betting + (max_betting-min_betting)*percent);
        gamePlayers[MAIN_USER].betting = cur_betting;
        
        updateBettingLabel();
    }
}

void GameScene_Online::onBettingChecked(Ref *sender, Widget::TouchEventType type, int btn_id)
{
    if(timer_index != MAIN_USER) return;
    
	if (type == Widget::TouchEventType::ENDED) {
        
        if (g_bSound == true) {

            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        stopTimerCountdown();
        
        if (btn_id == 1) { //if max betting
            
            cur_betting = max_betting;
        }
        
        gamePlayers[MAIN_USER].betting = cur_betting;
        sendInfo();
        
        //CocosDenshion::SimpleAudioEngine::getInstance()->stopEffect(timersound_index);
        CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
        activeAllLights(true);
        panelBetting->setVisible(false);
    }
}

void GameScene_Online::onProfileClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
    if (type == Widget::TouchEventType::ENDED) {
     
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        if(gamePlayers[btn_id].flagEmpty == true) {
            
            if (g_nRoomType == MASTER_ROOM) {
                
                pTableInviteLayer->showLayer(btn_id);
            }
            
            return;
        }
        
        if (panelBetting->isVisible() == true) {
            
            return;
        }
        
        gamePlayers[btn_id].info.user_id = gamePlayers[btn_id].user_id;
        gamePlayers[btn_id].info.photo_id = gamePlayers[btn_id].photo_id;
        gamePlayers[btn_id].info.user_name = gamePlayers[btn_id].name;
        //gamePlayers[btn_id].info.points = gamePlayers[btn_id].points;
        gamePlayers[btn_id].info.level = gamePlayers[btn_id].level;
        
        pPublicProfileLayer->showLayer(gamePlayers[btn_id].info, btn_id);
        
        //get profile data
        std::string strURL = StringUtils::format("http://%s:8080/sys/member/mobile/getUserInfo.do", WEBSERVICE_HOST);
        
        HttpRequest* request = new HttpRequest();
        request->setRequestType(HttpRequest::Type::POST);
        request->setUrl(strURL.c_str());
        request->setResponseCallback(CC_CALLBACK_2(GameScene_Online::onGetPublicProfileDone, this, btn_id));
        
        // write the post data
        std::string strData = StringUtils::format("USER_ID=%s&FB_ID=%s",
                                                  gamePlayers[btn_id].user_id.c_str(),
                                                  g_mainUserInfo.user_id.c_str()
                                                  );
        request->setRequestData(strData.c_str(), strData.length());
        
        request->setTag("get Public Profile");
        //HttpClient::getInstance()->send(request);
        HttpClient::getInstance()->sendImmediate(request);
        
        request->release();
    }
}

void GameScene_Online::onGetPublicProfileDone(HttpClient* sender, HttpResponse* response, int pos)
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
    
    userInfo.friend_flag = cJSON_GetObjectItem(data, "friend")->valueint;
    
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
    
    cJSON_Delete(json);

    if(gamePlayers[pos].flagEmpty == true) return;
    if(gamePlayers[pos].user_id.compare(userInfo.user_id) != 0) return;
    
    gamePlayers[pos].info = userInfo;
    
    if (pPublicProfileLayer->isVisible() == true && pPublicProfileLayer->showing_pos == pos) {
        
        pPublicProfileLayer->showLayer(userInfo, pos);
    }
}

void GameScene_Online::onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
	//if ( isShowHelp == true ) return;

	if (type == Widget::TouchEventType::ENDED) {

        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
		if (btn_id == 0) { //back

            exitGame();
		}
		else if (btn_id == 1) { //sound

            g_bSound = !g_bSound;
            cocos2d::UserDefault::getInstance()->setBoolForKey(SOUND, g_bSound);
            if(g_bSound == false) {
                
                CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
                CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
                btn_sound->loadTextures("btn_soundoff.png", "btn_soundoff_.png", "btn_soundoff_.png", TextureResType::PLIST);
            }
            else {
                
                btn_sound->loadTextures("btn_sound.png", "btn_sound_.png", "btn_sound_.png", TextureResType::PLIST);
            }
		}
		else if (btn_id == 2) { //store

            pStoreLayer->showLayer();
		}
	}
    else if (btn_id == 3) { //chat
        
        pChatLayer->showLayer();
    }
    else if (btn_id == 4) { //fold
        
        stopTimerCountdown();
        
        panelAction->setVisible(false);
        img_hands[MAIN_USER]->setVisible(false);
        hideShowCard();
        
        gamePlayers[MAIN_USER].call_type = FOLD_PLAY;
        
        sendInfo();
        
        CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
        //CocosDenshion::SimpleAudioEngine::getInstance()->stopEffect(timersound_index);
        activeAllLights(true);
    }
    else if (btn_id == 5) { //bet
        
//        if(gamePlayers[MAIN_USER].points > g_nPlayFee) {
//            
//            min_betting = g_nPlayFee;
//            max_betting = game_pot;
//        }
//        else {
//            
//            min_betting = gamePlayers[MAIN_USER].points;
//            max_betting = min_betting;
//        }
//        
//        if(min_betting > max_betting) {
//            
//            min_betting = max_betting;
//        }
        
        pPublicProfileLayer->hideLayer();
        
        max_betting = MIN(gamePlayers[MAIN_USER].points, game_pot);
        min_betting = MIN(max_betting, g_nPlayFee);
        
        cur_betting = min_betting;
        slider_betting->setPercent(0);
        
        updateBettingLabel();
        
        panelAction->setVisible(false);
        panelBetting->setVisible(true);
        
        gamePlayers[MAIN_USER].call_type = BETTING_PLAY;
        gamePlayers[MAIN_USER].betting = cur_betting;
    }
    else if(btn_id == 6) { //deck panel
        
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("barajear.aac", false);
        }
    }
}

void GameScene_Online::exitGame()
{
    exitFlag = true;
    GameController::getInstance()->disconnect2AppWarp();
    
    CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
    CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    
    Scene *pScene = TransitionFade::create(0.5, HelloWorld::createScene());
    Director::getInstance()->replaceScene(pScene);
    
//    DelayTime *delayTime = DelayTime::create(0.2f);
//    CallFunc *call_func = CallFunc::create(CC_CALLBACK_0(GameScene_Online::actionLeaveGameDone, this));
//    Sequence *delayAct = Sequence::create(delayTime, call_func, NULL);
//    this->runAction(delayAct);
}

void GameScene_Online::actionLeaveGameDone()
{
    
}

bool GameScene_Online::checkValidSelection(int card_index)
{
	return true;
}

void GameScene_Online::showErrorMsg(std::string msg)
{
    //AppService::showCircleProgressbar();
    
//    cocos2d::MessageBox(msg.c_str(), "");
    //pAlertLayer->showLayer("Oops !", msg);
    
    if (isEnablePlay == false) {
        
        pAlertLayer->showLayer("Oops !", msg);
        
        DelayTime *delayTime = DelayTime::create(1.0f);
        CallFunc *call_func = CallFunc::create(CC_CALLBACK_0(GameScene_Online::exitGame, this));
        Sequence *delayAct = Sequence::create(delayTime, call_func, NULL);
        this->runAction(delayAct);
    }
    else {
    
        pConnectingLayer->showLayer();
        
        DelayTime *delayTime = DelayTime::create(20.0f);
        CallFunc *call_func = CallFunc::create(CC_CALLBACK_0(GameScene_Online::exitGame, this));
        Sequence *delayAct = Sequence::create(delayTime, call_func, NULL);
        this->runAction(delayAct);
    }
    
}

void GameScene_Online::showEffectMsg(int type)
{
//	pEffectLayer->showLayer(type);
//
//	DelayTime *delayTime = CCDelayTime::create(3.0f);
//	CallFunc *call_func = CallFunc::create(CC_CALLBACK_0(EffectLayer::hideLayer, pEffectLayer));
//	Sequence *delayAct = Sequence::create(delayTime, call_func, NULL);
//	this->runAction(delayAct);
}

void GameScene_Online::showRoundAlarm()
{
//    pRoundAlarmLayer->showLayer(numRounds+1);
//    
//    DelayTime *delayTime = CCDelayTime::create(1.0f);
//    CallFunc *call_func = CallFunc::create(CC_CALLBACK_0(RoundAlarmLayer::hideLayer, pRoundAlarmLayer));
//    Sequence *delayAct = Sequence::create(delayTime, call_func, NULL);
//    this->runAction(delayAct);
}

void GameScene_Online::showActiveMark(int player_id)
{
	for (int i = 0; i < MAX_PLAYERS; i++) {

		if (i == player_id) {

			img_active[i]->setVisible(true);
		}
		else {

			img_active[i]->setVisible(false);
		}
	}
}

void GameScene_Online::hideActiveMark()
{
	for (int i = 0; i < MAX_PLAYERS; i++) {

		img_active[i]->setVisible(false);
	}
}

void GameScene_Online::initTimerCountdown(Node *rootNode)
{
    //init progress timer
    auto spriteTimer_0 = Sprite::createWithSpriteFrameName("timer_bg0.png");
    auto spriteTimer_1 = Sprite::createWithSpriteFrameName("timer_bg1.png");
    
    for (int i = 0; i < MAX_PLAYERS; i++) {
        
        if(i == MAIN_USER) {
        
            timerProgress[i] = CCProgressTimer::create(spriteTimer_0);
        }
        else {
         
            timerProgress[i] = CCProgressTimer::create(spriteTimer_1);
        }
        //Set the type
        //heart_->setType(kCCProgressTimerTypeBar);
        timerProgress[i]->setType(kCCProgressTimerTypeRadial);
        
        //The starting point from which to change direction
        timerProgress[i]->setMidpoint(ccp(0.5, 0.5));
        
        //ccp(1, 0) Horizontal CCP (0, 1) in vertical direction
        timerProgress[i]->setBarChangeRate(ccp(1, 1));
        
        timerProgress[i]->setAnchorPoint(Vec2(0.5, 0.5));
        timerProgress[i]->setScaleX(-1);
        Vec2 tar_pos = img_active[i]->getPosition();
        //tar_pos.x *= getScaleValX();
        //tar_pos.y *= getScaleValY();
        timerProgress[i]->setPosition(tar_pos);
        //timerProgress[i]->setPosition(Vec2(panelProfile[i]->getBoundingBox().size.width*0.5, panelProfile[i]->getBoundingBox().size.height*0.5));
        //timerProgress[i]->setScaleX(g_fScaleWH);
        panelProfile[i]->addChild(timerProgress[i]);
        timerProgress[i]->setLocalZOrder(0);
        
        timerProgress[i]->setColor(Color3B::RED);
        
        /*if(i == MAIN_USER) {
            
            timerProgress[i]->setScale(getScaleValX()*1.3, getScaleValY()*1.3);
        }
        else {
            
            timerProgress[i]->setScale(getScaleValX(), getScaleValY());
        }*/
        timerProgress[i]->setVisible(false);
        //rootNode->addChild(timerProgress[i]);
    }
}

void GameScene_Online::runTimerCountdown(int player_id)
{
    if (timerProgress[player_id]->isVisible()) {
        
        return;
    }
    
    stopTimerCountdown();
    
    timerProgress[player_id]->setColor(Color3B::GREEN);
    //The loading action, setting 5 seconds loaded into the 100%
    //CCProgressTo *progressTo = CCProgressTo::create(15, 0);
    timerProgress[player_id]->setVisible(true);
    CCProgressFromTo *progressFromTo = CCProgressFromTo::create(TURN_TIME, 100, 0);
    //heart_->runAction(CCRepeatForever::create(progressFromTo));
    timerProgress[player_id]->runAction(CCSequence::create(progressFromTo, NULL));
    
    timer_index = player_id;
    durationTimer = 0;
}

void GameScene_Online::stopTimerCountdown()
{
    timer_index = INVALID;
    durationTimer = 0;
    for (int i = 0; i < MAX_PLAYERS; i++) {
        
        timerProgress[i]->stopAllActions();
        //timerProgress[i]->setPercentage(0);
        timerProgress[i]->setVisible(false);
    }
}

void GameScene_Online::run()
{
	switch (gameStatus) {

	case BEGIN_STATUS:

		resetGame();
		break;
            
    case RISK_STATUS:
        
        risking();
        break;

	case DEAL_STATUS:

		dealing();
		break;

	case PLAY_STATUS:

		play();
		break;

	default:
		break;
	}
}

void GameScene_Online::startGame()
{
//    if (g_nRoomType == VIP_ROOM) {
//        
//        g_mainUserInfo.points += 65;
//    }
//    else if(g_nRoomType == MASTER_ROOM) {
//            
//        g_mainUserInfo.points -= 65;
//    }
//    else {
//            
//       g_mainUserInfo.points -= 50;
//    }
//    g_mainUserInfo.total_play++;
//    saveProfileData();
//    WebService::getInstance()->updateDiamond();
    
    //pWaitingLayer->hideLayer();
    
	gameStatus = BEGIN_STATUS;

	run();
}

void GameScene_Online::resetGame()
{
    showRoundAlarm();
	
    Vec2 posDealer = img_deck->getPosition();
	for (int i = 0; i < NUM_CARDS; i++) {

        img_backCards[i]->setPosition(posDealer);
		img_backCards[i]->setVisible(true);
	}

    for (int i = 0; i < MAX_PLAYERS; i++) {
        
        img_hands[i]->loadTexture("pcard_back.png", TextureResType::PLIST);
    }

    stopWinLoseAnimation();
	hideActiveMark();
    showDealerMark();
    
    checkTotalPoint();
    
    GameController::getInstance()->endProcessing();

//	for (int p = 0; p < MAX_PLAYERS; p++) {
//
//		gamePlayers[p].init();
//
////		for (int k = 0; k < CARDS_PER_PLAYER; k++) {
////
////			int index = p*CARDS_PER_PLAYER + k;
////			spriteCards_back[index]->setVisible(true);
////		}
//	}

//    lb_round->setString(StringUtils::format("Round %d", numRounds+1));
//	itsDeck.init();
//	Table::getInstance()->init();
//	updateScore();
//
//	isEnableSelect = false;
//
//	numTricks = 0;
//	numRounds = 0;
//    cntDealingCards = 0;
}

void GameScene_Online::actionResetDone()
{
    stopTimerCountdown();
    numCardsInHand = 0;
	gameStatus = DEAL_STATUS;

	if (g_bSound == true)
	{
		//CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("shuffle.aac", false, 1, 0, 10);
	}
	run();
}

void GameScene_Online::endMove(int player_id, int backcard_id, int index, int frontcard_id)
{
//    spriteCards_back[backcard_id]->setVisible(false);
//    Point tar_pos = img_hands[player_id][index]->getPosition();
//    float angle = img_hands[player_id][index]->getRotation();
//    float scaleX = img_hands[player_id][index]->getScaleX();
//    float scaleY = img_hands[player_id][index]->getScaleY();
//    
//    //flip animation
//    spriteCards[frontcard_id]->setPosition(tar_pos);
//    spriteCards[frontcard_id]->setVisible(true);
//    spriteCards[frontcard_id]->runAction(Sequence::create(
//         ScaleTo::create(0.2, 0, scaleY),
//         CallFunc::create([=]()->void{
//            spriteCards[frontcard_id]->setSpriteFrame(StringUtils::format("%02d.png", frontcard_id));
//         }),
//         ScaleTo::create(0.2, scaleX, scaleY),
//         NULL));
}

void GameScene_Online::endMove_1(int player_id, int backcard_id, int index)
{
//    spriteCards_back[backcard_id]->setVisible(false);
//    img_hands[player_id][index]->setVisible(true);
}

void GameScene_Online::actionSetCardTexture(ImageView *imgCard, std::string file)
{
    imgCard->loadTexture(file, TextureResType::PLIST);
}

void GameScene_Online::FlipActionEffect(ImageView *imgCard, std::string file)
{
    imgCard->setVisible(true);
    float scaleX = imgCard->getScaleX();
    float scaleY = imgCard->getScaleY();
    Sequence * act1 = Sequence::create(
                                       ScaleTo::create(0.2f, 0, scaleY),
                                       CallFunc::create(CC_CALLBACK_0(GameScene_Online::actionSetCardTexture, this, imgCard, file)),
                                       ScaleTo::create(0.2f, scaleX, scaleY),
                                       NULL);
    imgCard->runAction(act1);
}

void GameScene_Online::actionAnimation2Pot(int index)
{
    imgAnimationChips[index]->setVisible(false);
    
    updatePot();
    lb_point[index]->setString(getNormalText2Number(gamePlayers[index].points));
    
    if(g_bSound == true)
    {
        //CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("risking.aac", false);
    }
}

void GameScene_Online::risking()
{
    for (int i = 0; i < MAX_PLAYERS; i++) {
        
        if (gamePlayers[i].flagEmpty == true) continue;
        if (gamePlayers[i].activeStatus == INVALID) continue;
        
        if(i == MAIN_USER) {
        
            g_mainUserInfo.points -= g_nPlayFee;
            WebService::getInstance()->updatePoint();
        }
        
        Vec2 tar_pos = img_pot->getPosition();
        
        lb_AnimationChips[i]->setString(StringUtils::format("%d", g_nPlayFee));
        setChipTextureWithPoint(imgAnimationChips[i], g_nPlayFee);
        imgAnimationChips[i]->setPosition(panelProfile[i]->getPosition());
        imgAnimationChips[i]->setOpacity(255);
        imgAnimationChips[i]->setVisible(true);
        
        imgAnimationChips[i]->runAction(Sequence::create( DelayTime::create(0.05),
                                                          MoveTo::create(0.5f, tar_pos),
                                                          //FadeOut::create(0.0),
                                                          CallFunc::create(CC_CALLBACK_0(GameScene_Online::actionAnimation2Pot, this, i)),
                                                          NULL));
    }
    
    GameController::getInstance()->endProcessing();
}

void GameScene_Online::dealing()
{
    int cnt = 0;
    for(int i = 0; i < MAX_PLAYERS; i++) {
        
        if(gamePlayers[i].flagEmpty == true) continue;
        if(gamePlayers[i].activeStatus == INVALID) continue;
        
        Point tar_pos = img_hands[i]->getPosition();
        img_backCards[i]->runAction(Sequence::create(
                                               DelayTime::create(cnt*0.3),
                                               MoveTo::create(0.4f, tar_pos),
                                               //FadeOut::create(0.0),
                                               CallFunc::create([=]()->void{
            
                                                    img_backCards[i]->setVisible(false);
                                                    img_hands[i]->setVisible(true);
                                                    if (g_bSound) {
                                                        
                                                        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("pcard_deal.aac", false);
                                                    }
                                               }),
                                               //CallFunc::create(CC_CALLBACK_0(GameScene_Online::FlipActionEffect, this, img_hands[i], file)),
                                               NULL));
        
        cnt++;
    }

    activePlayer = dealer;
    
    GameController::getInstance()->endProcessing();
    
//	DelayTime *delayTime = CCDelayTime::create(0.2);
//	CallFunc *call_func = CallFunc::create(CC_CALLBACK_0(GameScene_Online::actionDealDone, this));
//	Sequence *delayAct = Sequence::create(delayTime, call_func, NULL);
//	this->runAction(delayAct);
}

void GameScene_Online::sendInfo()
{
    std::string msg = StringUtils::format("%d:%.0f:", gamePlayers[MAIN_USER].call_type, gamePlayers[MAIN_USER].betting);
    GameController::getInstance()->sendChatMessage(msg);
}

void GameScene_Online::actionDealDone()
{
    numCardsInHand++;
    cntDealingCards++;
	if (numCardsInHand == CARDS_PER_PLAYER*MAX_PLAYERS) {
        
        return;
	}

	run();
}

void GameScene_Online::actionDealComplete()
{
    numCardsInHand = 0;
	gameStatus = PLAY_STATUS;
	activePlayer = (dealer + 1) % MAX_PLAYERS;

	run();
}

void GameScene_Online::play()
{
    stopWinLoseAnimation();
    
	//showActiveMark(activePlayer);
    gamePlayers[activePlayer].call_type = INVALID;
    gamePlayers[activePlayer].betting = 0;

    showPurpleCacrd(activePlayer);
	if (activePlayer == MAIN_USER) {
		
        stopTimerCountdown();
        durationTimer = 0;
        timer_index = MAIN_USER;
        
        if (g_bSound == true) {
            
            //timersound_index = CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("main_timer.aac", false);
            CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("main_timer.aac", false);
        }
        panelAction->setVisible(true);
	}
    else {
        
       runTimerCountdown(activePlayer);
    }
    
    GameController::getInstance()->endProcessing();
}

void GameScene_Online::actionSelection()
{
//    int card_index;
//    Point tar_pos;
//    float scaleX, scaleY;
//    
//    Card card = getCardFromID(cur_playing.hand_card);
//    card_index = cur_playing.hand_card;
//    
//    isEnableSelect = false;
//    
//    //animation effect
//    if (activePlayer != MAIN_USER) {
//        
//        int index = INVALID;
//        for (int i = 0; i < CARDS_PER_PLAYER; i++) {
//            
//            if(img_hands[activePlayer][i]->isVisible() == false) continue;
//            
//            index = i;
//            break;
//        }
//        
//        scaleX = img_hands[activePlayer][index]->getScaleX();
//        scaleY = img_hands[activePlayer][index]->getScaleY();
//        img_hands[activePlayer][index]->setVisible(false);
//        spriteCards[card_index]->setPosition(img_hands[activePlayer][index]->getPosition());
//        spriteCards[card_index]->setRotation(img_hands[activePlayer][index]->getRotation());
//        spriteCards[card_index]->setVisible(true);
//        spriteCards[card_index]->runAction(Sequence::create(
//            ScaleTo::create(0.2, 0, scaleY),
//            CallFunc::create([=]()->void{
//
//                spriteCards[card_index]->setSpriteFrame(StringUtils::format("%02d.png", card_index));
//            }),
//            ScaleTo::create(0.2, scaleX, scaleY),
//            NULL));
//    }
//    
//    //selection
//    //spriteCards[card_index]->setColor(Color3B::ORANGE);
//    spriteCards_d[card_index]->setVisible(true);
//    if (cur_playing.type != DISCARD_PLAY) {
//        
//        for (int i = 0; i < cur_playing.vCommonCards.size(); i++) {
//            
//            card_index = cur_playing.vCommonCards[i];
//            //spriteCards[card_index]->setColor(Color3B::ORANGE);
//            spriteCards_d[card_index]->setVisible(true);
//        }
//    }
}

void GameScene_Online::moveSelectionCards()
{
//    int card_index;
//    Point tar_pos;
//    float angle, scaleX, scaleY;
//    
//    updateScore();
//    
//    Card card = getCardFromID(cur_playing.hand_card);
//    card_index = cur_playing.hand_card;
//    
//    if (cur_playing.type == DISCARD_PLAY) { //if discarding
//        
//        int index = Table::getInstance()->addCard(card);
//        tar_pos = img_discard[index]->getPosition();
//        angle = img_discard[index]->getRotation();
//        scaleX = img_discard[index]->getScaleX();
//        scaleY = img_discard[index]->getScaleY();
//        
//        //animation effect
//        //spriteCards[card_index]->setColor(Color3B::WHITE);
//        spriteCards_d[card_index]->setVisible(false);
//        spriteCards[card_index]->runAction(Sequence::create(
//                                                            Spawn::create(
//                                                                          MoveTo::create(0.3f, tar_pos),
//                                                                          RotateTo::create(0.3f, angle),
//                                                                          NULL),
//                                                            CallFunc::create([=]()->void{
//            
//            spriteCards[card_index]->setZOrder(index + 1);
//        }),
//                                                            NULL));
//       
//        return;
//    }
//    
//    //if matching or addition
//    tar_pos = img_center->getPosition();
//    for (int i = 0; i < cur_playing.vCommonCards.size(); i++) {
//        
//        int index = cur_playing.vCommonCards[i];
//        
//        spriteCards[index]->runAction(Sequence::create(
//                                                       MoveTo::create(0.3f, tar_pos),
//                                                       CallFunc::create([=]()->void{
//            spriteCards[index]->setZOrder(15);
//        }),
//                                                       NULL));
//    }
//    
//    angle = img_center->getRotation();
//    scaleX = img_center->getScaleX();
//    scaleY = img_center->getScaleY();
//    
//    spriteCards[card_index]->runAction(Sequence::create(
//                                                        Spawn::create(
//                                                                      MoveTo::create(0.3f, tar_pos),
//                                                                      RotateTo::create(0.3f, angle),
//                                                                      NULL),
//                                                        NULL));
}

void GameScene_Online::updatePlayerPointLabel(int pos)
{
    lb_point[pos]->setString(getNormalText2Number(gamePlayers[activePlayer].points));
}

void GameScene_Online::updateBettingLabel()
{
    lb_betting->setString(getNormalText2Number(cur_betting));
}

void GameScene_Online::stopWinLoseAnimation()
{
    CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
    hideShowCard();
    
    for (int i = 0; i < 23; i++) {
        
        particle_lights[i]->stopSystem();
    }
    
    for (int i = 0; i < 4; i++) {
        
        particle_chip[i]->stopSystem();
    }
    for (int i = 0; i < 4; i++) {
        
        particle_win[i]->stopSystem();
    }
    for (int i = 0; i < 2; i++) {
        
        particle_lose[i]->stopSystem();
    }
    
    particle_cleanpot->stopSystem();
    
    img_winmark->setVisible(false);
    img_losemark->setVisible(false);
}

void GameScene_Online::initTextAnimation()
{
    curValue = 0;
    deltaValue = 0;
    cntTextTimer = 0;
    posTextAnimation = INVALID;
    flagTextAnimation = false;
}

void GameScene_Online::updateLevelPoints()
{
    if(g_mainUserInfo.level == MAX_LEVEL) return;
    
    //calculate level points
    int xp = 8;
    int pcard_index = gamePlayers[MAIN_USER].purple_card%10;
    if(pcard_index < 6) {
        
        xp = 18;
    }
    
    //xp *= 100;
    g_mainUserInfo.level_point += xp;
    if(g_mainUserInfo.level_point >= g_nLevelupXP[g_mainUserInfo.level]) {
     
        g_mainUserInfo.level++;
        g_mainUserInfo.level_point = 0;
        
        gamePlayers[MAIN_USER].level = g_mainUserInfo.level;
        lb_level[MAIN_USER]->setString(StringUtils::format("%d", g_mainUserInfo.level+1));
        
        //show levelup layer
        pLevelupLayer->showLayer(g_mainUserInfo.level);
        
        DelayTime *delayTime = DelayTime::create(2.0f);
        CallFunc *call_func = CallFunc::create(CC_CALLBACK_0(LevelupLayer::hideLayer, pLevelupLayer));
        Sequence *delayAct = Sequence::create(delayTime, call_func, NULL);
        this->runAction(delayAct);
    }
}

void GameScene_Online::actionWinAnimation(int pos)
{
    imgAnimationChips[pos]->setVisible(false);
    
    //set parameter for text animation
    initTextAnimation();
    curValue = gamePlayers[pos].points;
    deltaValue = gamePlayers[pos].betting/100.0f;
    posTextAnimation = pos;
    flagTextAnimation = true;
    /////////////////////////////////////////////
    
    game_pot -= gamePlayers[pos].betting;
    updatePot();
    
    if(pos == MAIN_USER) {
        
        updateLevelPoints();
        
        g_mainUserInfo.points += gamePlayers[pos].betting;
        
        int clean_pot = 1;
        if(game_pot > 0) {
            
            clean_pot = 0;
        }
        win_streak++;
        
        WebService::getInstance()->updatePlayInfo(gamePlayers[pos].betting, playtime, clean_pot, win_streak);
        
        playtime = 0;
        //cocos2d::UserDefault::getInstance()->setFloatForKey("point", g_mainUserInfo.points);
    }

    gamePlayers[pos].points += gamePlayers[pos].betting;
    gamePlayers[pos].betting = 0;
    //updatePlayerPointLabel(pos);
    
    img_winmark->setVisible(true);
    
    GameController::getInstance()->endProcessing();
}

void GameScene_Online::actionLoseAnimation(int pos)
{
    imgAnimationChips[pos]->setVisible(false);
    
    //set parameter for text animation
//    initTextAnimation();
//    curValue = gamePlayers[pos].points;
//    deltaValue = -gamePlayers[pos].betting/100.0f;
//    posTextAnimation = pos;
//    flagTextAnimation = true;
    /////////////////////////////////////////////
    
    game_pot += gamePlayers[pos].betting;
    updatePot();
    
    if(pos == MAIN_USER) {
        
        g_mainUserInfo.points -= gamePlayers[pos].betting;
        if (g_mainUserInfo.points < 0) {
            
            g_mainUserInfo.points = 0;
        }
        
        win_streak = 0;
        
        WebService::getInstance()->updatePlayInfo(0, playtime, 0, win_streak);
        
        playtime = 0;
        //cocos2d::UserDefault::getInstance()->setFloatForKey("point", g_mainUserInfo.points);
    }
    
    gamePlayers[pos].points -= gamePlayers[pos].betting;
    gamePlayers[pos].betting = 0;
    
    updatePlayerPointLabel(pos);
    
    img_losemark->setVisible(true);
    
    GameController::getInstance()->endProcessing();
}

void GameScene_Online::setChipTextureWithPoint(ImageView *img_chip, float point)
{
    std::string file;
    int index = 0;
    if(point > 1000) {
        
        index = 6;
    }
    else if(point > 100) {
        
        index = 5;
    }
    else if(point > 50) {
        
        index = 4;
    }
    else if(point > 20) {
        
        index = 3;
    }
    else if(point > 10) {
        
        index = 2;
    }
    else if(point > 5) {
        
        index = 1;
    }

    img_chip->loadTexture(StringUtils::format("chip_%d.png", index), TextureResType::PLIST);
}

void GameScene_Online::actionPlayCompleted(int res)
{
    CCLOG("actionPlayCompleted: %d", res);
    
    img_chips[activePlayer]->setVisible(false);
    img_hands[activePlayer]->setVisible(false);
    
    if(res == 1) { //win
        
        if(activePlayer == MAIN_USER && g_bSound == true) {
            
            int pcard_index = gamePlayers[activePlayer].purple_card%10;
            //CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
            if ((game_pot - gamePlayers[activePlayer].betting) < 0) {
                
                CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("clean_pot.aac", false);
            }
            else if (gamePlayers[activePlayer].betting > 3000) {
                
                CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("win_3000.aac", false);
            }
            else if (pcard_index < 5) {
                
                CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("win_1.aac", false);
            }
            else {
                
                CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("win_2.aac", false);
            }
        }
        
        //winner mark
        img_winmark->setPosition(panelProfile[activePlayer]->getPosition());
        img_winmark->setVisible(true);
        
        //particles
        if(activePlayer == MAIN_USER) {
           
            //chip animation
            for(int i = 0; i < 4; i++) {
                
                particle_chip[i]->resetSystem();
            }
            //shine aniamtion
            particle_win[0]->resetSystem();
            
            //if clean pot, show special effect
            if (gamePlayers[activePlayer].betting == game_pot) {
                
                particle_cleanpot->resetSystem();
            }
        }
        else {
            
            particle_win[1]->setPosition(panelProfile[activePlayer]->getPosition());
            particle_win[1]->resetSystem();
            particle_win[2]->setPosition(panelProfile[activePlayer]->getPosition());
            particle_win[2]->resetSystem();
            particle_win[3]->setPosition(panelProfile[activePlayer]->getPosition());
            particle_win[3]->resetSystem();
        }
        
        //move chip from pot to player
        Vec2 tar_pos = panelProfile[activePlayer]->getPosition();
        
        imgAnimationChips[activePlayer]->setPosition(img_pot->getPosition());
        imgAnimationChips[activePlayer]->setOpacity(255);
        imgAnimationChips[activePlayer]->setVisible(true);
        lb_AnimationChips[activePlayer]->setString(StringUtils::format("%.0f", gamePlayers[activePlayer].betting));
        
        imgAnimationChips[activePlayer]->runAction(Sequence::create( DelayTime::create(0.05),
                                                         MoveTo::create(0.8f, tar_pos),
                                                         //FadeOut::create(0.0),
                                                         CallFunc::create(CC_CALLBACK_0(GameScene_Online::actionWinAnimation, this, activePlayer)),
                                                         NULL));
    }
    else { //lose
        
        int gcard_index;
        if(activePlayer == MAIN_USER) {
         
            if(green_card > 39) {
                
                if (g_bSound == true) {
                    
                    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("gcard.aac", false);
                }
            }
            else {
                
                //gcard_index = (green_card%10) + 1;
                if (g_bSound == true) {
                    
//                    if(gcard_index == 1 || gcard_index == 10) {
//                        
//                        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("gcard.aac", false);
//                    }
//                    else
                    {
                        
                        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("lose.aac", false);
                    }
                }
            }
        }
        
        for (int i = 0; i < 23; i++) {
            particle_lights[i]->setDuration(3.0f);
            
            particle_lights[i]->resetSystem();
        }
        
        //looser mark
        img_losemark->setPosition(panelProfile[activePlayer]->getPosition());
        img_losemark->setVisible(true);
        
        if(activePlayer == MAIN_USER) {
            
//            particle_lose[0]->setPosition(img_pot->getPosition());
//            particle_lose[0]->resetSystem();
        }
        else {
            
            particle_lose[1]->setPosition(panelProfile[activePlayer]->getPosition());
            particle_lose[1]->resetSystem();
        }
        
        //move chip from player to pot
        Vec2 tar_pos = img_pot->getPosition();
        
        imgAnimationChips[activePlayer]->setPosition(img_chips[activePlayer]->getPosition());
        imgAnimationChips[activePlayer]->setOpacity(255);
        imgAnimationChips[activePlayer]->setVisible(true);
        lb_AnimationChips[activePlayer]->setString(StringUtils::format("%.0f", gamePlayers[activePlayer].betting));
        
        imgAnimationChips[activePlayer]->runAction(Sequence::create( DelayTime::create(0.05),
                                                                    MoveTo::create(0.3f, tar_pos),
                                                                    //FadeOut::create(0.0),
                                                                    CallFunc::create(CC_CALLBACK_0(GameScene_Online::actionLoseAnimation, this, activePlayer)),
                                                                    NULL));
    }
    
    //go to next step
//    activePlayer = (activePlayer + 1) % MAX_PLAYERS;
//    GameController::getInstance()->endProcessing();
//    if (numCardsInHand == 0) {
//        
//        stopTimerCountdown();
//        return;
//    }
//    
//    play();
}

void GameScene_Online::actionPlayDone()
{
    stopTimerCountdown();
    if(gamePlayers[activePlayer].call_type == FOLD_PLAY) {
        
        if(activePlayer == MAIN_USER) {
            
            activeAllLights(true);
            
            win_streak = 0;
            WebService::getInstance()->updatePlayInfo(0, playtime, 0, win_streak);
            playtime = 0;
        }

        panelAction->setVisible(false);
        panelBetting->setVisible(false);
        img_hands[activePlayer]->setVisible(false);
        hideShowCard();
        GameController::getInstance()->endProcessing();
        return;
    }
    
    setChipTextureWithPoint(imgAnimationChips[activePlayer], gamePlayers[activePlayer].betting);
    setChipTextureWithPoint(img_chips[activePlayer], gamePlayers[activePlayer].betting);
    imgAnimationChips[activePlayer]->setPosition(panelProfile[activePlayer]->getPosition());
    imgAnimationChips[activePlayer]->setVisible(true);
    lb_AnimationChips[activePlayer]->setString(StringUtils::format("%.0f", gamePlayers[activePlayer].betting));
    
    Point tar_pos = img_chips[activePlayer]->getPosition();
    imgAnimationChips[activePlayer]->runAction(Sequence::create( DelayTime::create(0.05),
                                                     MoveTo::create(0.5f, tar_pos),
                                                     CallFunc::create([=]()->void{
                                                        imgAnimationChips[activePlayer]->setVisible(false);
                                                        img_chips[activePlayer]->setVisible(true);
                                                        lb_chips[activePlayer]->setString(StringUtils::format("%.0f", gamePlayers[activePlayer].betting));
                                                     }),
                                                     NULL));
    
    GameController::getInstance()->endProcessing();
    
    
//    int card_index;
//    Point tar_pos;
//    float angle, scaleX, scaleY;
    
//    updateScore();
//    
//    Card card = getCardFromID(cur_playing.hand_card);
//    card_index = cur_playing.hand_card;
//    
//    numCardsInHand--;
//    if (cur_playing.type == DISCARD_PLAY) { //if discarding
//        
//        if (activePlayer != MAIN_USER) {
//            
//            int index = Table::getInstance()->addCard(card);
//            tar_pos = img_discard[index]->getPosition();
//            angle = img_discard[index]->getRotation();
//            scaleX = img_discard[index]->getScaleX();
//            scaleY = img_discard[index]->getScaleY();
//            
//            //animation effect
//            //spriteCards[card_index]->setColor(Color3B::WHITE);
//            spriteCards_d[card_index]->setVisible(false);
//            spriteCards[card_index]->runAction(Sequence::create(
//                                                                Spawn::create(
//                                                                              MoveTo::create(0.3f, tar_pos),
//                                                                              RotateTo::create(0.3f, angle),
//                                                                              NULL),
//                                                                CallFunc::create([=]()->void{
//                
//                spriteCards[card_index]->setZOrder(index + 1);
//            }),
//                                                                NULL));
//        }
//        
//        //remove cards from hand and table
//        removeHandCard(activePlayer, cur_playing.hand_card);
//        
//        int delay_time = 0;
//        if(effect_type != INVALID) {
//            
//            delay_time = 3;
//            showEffectMsg(effect_type);
//        }
//        
//        DelayTime *delayTime = DelayTime::create(delay_time + 0.2);
//        CallFunc *call_func = CallFunc::create(CC_CALLBACK_0(GameScene_Online::actionPlayCompleted, this));
//        Sequence *delayAct = Sequence::create(delayTime, call_func, NULL);
//        this->runAction(delayAct);
//        
//        return;
//    }
//    
//    if (activePlayer == MAIN_USER) {
//        
//        actionPlayingEffectDone();
//        
//        return;
//    }
//    
//    //if matching or addition
//    tar_pos = img_center->getPosition();
//    for (int i = 0; i < cur_playing.vCommonCards.size(); i++) {
//        
//        int index = cur_playing.vCommonCards[i];
//        
//        spriteCards[index]->runAction(Sequence::create(
//                                                       MoveTo::create(0.3f, tar_pos),
//                                                       CallFunc::create([=]()->void{
//            spriteCards[index]->setZOrder(15);
//        }),
//                                                       NULL));
//    }
//    
//    angle = img_center->getRotation();
//    scaleX = img_center->getScaleX();
//    scaleY = img_center->getScaleY();
//    
//    spriteCards[card_index]->runAction(Sequence::create(
//                                                        Spawn::create(
//                                                                      MoveTo::create(0.3f, tar_pos),
//                                                                      RotateTo::create(0.3f, angle),
//                                                                      NULL),
//                                                        CallFunc::create(CC_CALLBACK_0(GameScene_Online::actionPlayingEffectDone, this)),
//                                                        NULL));
}

void GameScene_Online::putCard2Pile(int player_id, Card new_card)
{
	//put into taken pile
//	if (player_id == MAIN_USER || player_id == SOUTH) {
//
//		Table::getInstance()->vTakenPile[0].push_back(new_card);
//	}
//	else {
//
//		Table::getInstance()->vTakenPile[1].push_back(new_card);
//	}
}

//void GameScene_Online::actionTrickDone()
//{
//	hideActiveMark();
//	Table::getInstance()->last_index = INVALID;
//
//	gameStatus = DEAL_STATUS;
//	if (itsDeck.countTotal() == 0) {
//
//		bool flag = scoring();
//		if (flag == true) {
//
//			DelayTime *delayTime = CCDelayTime::create(0.1f);
//			CallFunc *call_func = CallFunc::create(CC_CALLBACK_0(GameScene_Online::actionRoundDone, this));
//			Sequence *delayAct = Sequence::create(delayTime, call_func, NULL);
//			this->runAction(delayAct);
//
//			return;
//		}
//
//		dealer = (dealer + 1) % MAX_PLAYERS;
//		gameStatus = BEGIN_STATUS;
//	}
//
//	DelayTime *delayTime = CCDelayTime::create(0.1f);
//	CallFunc *call_func = CallFunc::create(CC_CALLBACK_0(GameScene_Online::run, this));
//	Sequence *delayAct = Sequence::create(delayTime, call_func, NULL);
//	this->runAction(delayAct);
//}

void GameScene_Online::actionRoundDone()
{
//    for (int i = 0; i < NUM_CARDS; i++) {
//        
//        spriteCards[i]->setVisible(false);
//        spriteCards_d[i]->setVisible(false);
//    }
//    hideActiveMark();
//    
//    int num[12] = {4332, 1468, 1250, 1000, 700, 530, 350, 210, 110, 40, 6, 4};
//    
//    int value = INVALID;
//    if (Table::getInstance()->total_score[0] > Table::getInstance()->total_score[1]) {
//
//		//get random points
//        int index = 1;
//        value = rand()%10000;
//        int sum = 10000;
//        for (int i = 0; i < 12; i++) {
//            
//            sum = sum - num[11-i];
//            if(value >= sum) {
//                
//                index = 12-i;
//                break;
//            }
//        }
//        
//        value = 10*index;
//        g_mainUserInfo.win_play++;
//        g_mainUserInfo.points = value;
//	}
//    saveProfileData();
//    WebService::getInstance()->updatePlayInfo();
//    
//    std::vector<USER_INFO>vOthers;
//    for (int i = 0; i < MAX_PLAYERS/2; i++) {
//        
//        USER_INFO info;
//        info.photo_id = gamePlayers[2*i+1].photo_name;
//        info.user_name = gamePlayers[2*i+1].name;
//        vOthers.push_back(info);
//    }
//    //pResultLayer->showLayer(value, vOthers, g_nRoomType);
//    
//    if(g_nRoomType != MASTER_ROOM) {
//        
//        GameController::getInstance()->leaveGameRoom();
//    }
}

int GameScene_Online::getLocalPosition(int serverPos)
{
    if (serverPos == -1) {
        
        return -1;
    }
    
    int i = server_position;
    int k = MAIN_USER;
    while (1) {
        
        i = (i+1)%MAX_PLAYERS;
        k = (k+1)%MAX_PLAYERS;
        
        if (i == serverPos) {
            
            return k;
        }
    }
}

void GameScene_Online::onMasterRoomCreated()
{
    AppService::hideCircleProgressbar();
    
    for(int i = 0; i < MAX_PLAYERS; i++) {
        
        panelProfile[i]->setVisible(true);
    }
    
    isEnablePlay = true;
//    DelayTime *delayTime = DelayTime::create(0.1);
//    CallFunc *call_func = CallFunc::create(CC_CALLBACK_0(GameScene_Online::showFindTeamLayer, this));
//    Sequence *delayAct = Sequence::create(delayTime, call_func, NULL);
//    this->runAction(delayAct);
}

void GameScene_Online::onRPCMessage(std::string str_msg, int type)
{
    if(type == 0) { //request table info
        
        CCLOG("onRPCMessage");
        //get server_pos
        unsigned long loc = str_msg.find(':');
        std::string str_pos = str_msg.substr(0, loc);
        str_msg = str_msg.substr(loc+1);
        server_position = std::atoi(str_pos.c_str());
        //dealer
        loc = str_msg.find(':');
        std::string str_dealer = str_msg.substr(0, loc);
        str_msg = str_msg.substr(loc+1);
        dealer = getLocalPosition(std::atoi(str_dealer.c_str()));
        //active player
        loc = str_msg.find(':');
        std::string str_active = str_msg.substr(0, loc);
        str_msg = str_msg.substr(loc+1);
        activePlayer = getLocalPosition(std::atoi(str_active.c_str()));
        //server status
        loc = str_msg.find(':');
        std::string str_status = str_msg.substr(0, loc);
        str_msg = str_msg.substr(loc+1);
        serverStatus = std::atoi(str_status.c_str());
        //pot
        loc = str_msg.find(':');
        std::string str_pot = str_msg.substr(0, loc);
        str_msg = str_msg.substr(loc+1);
        game_pot = std::atof(str_pot.c_str());
        
        //get player active status
        for (int i = 0; i < MAX_PLAYERS; i++) {
            
            loc = str_msg.find(',');
            std::string str_useractive = str_msg.substr(0, loc);
            str_msg = str_msg.substr(loc+1);
            int local_pos = getLocalPosition(i);
            
            if (local_pos == MAIN_USER || str_useractive.empty() == true) continue;
            
            gamePlayers[local_pos].activeStatus = atoi(str_useractive.c_str());
        }
        
        img_table->loadTexture(StringUtils::format("table_%02d.png", server_position+1));
        initUserInfo(str_msg);
        updatePot();
        
        AppService::hideCircleProgressbar();
        
        isEnablePlay = true;
    }
}

void GameScene_Online::initUserInfo(std::string str_msg)
{
    int local_pos;
    unsigned long loc;
    
    //get player ids
    for (int i = 0; i < MAX_PLAYERS; i++) {
        
        loc = str_msg.find(',');
        std::string str_userid = str_msg.substr(0, loc);
        str_msg = str_msg.substr(loc+1);
        local_pos = getLocalPosition(i);
        
        if (local_pos == MAIN_USER || str_userid.empty() == true) continue;
        
        gamePlayers[local_pos].user_id = str_userid;
        gamePlayers[local_pos].flagEmpty = false;
    }
    //get player names
    for (int i = 0; i < MAX_PLAYERS; i++) {
        
        loc = str_msg.find(',');
        std::string str_username = str_msg.substr(0, loc);
        str_msg = str_msg.substr(loc+1);
        local_pos = getLocalPosition(i);
        
        if (local_pos == MAIN_USER || str_username.empty() == true) continue;
        
        gamePlayers[local_pos].name = str_username;
    }
    //get player photos
    for (int i = 0; i < MAX_PLAYERS; i++) {
        
        loc = str_msg.find(',');
        std::string str_userphoto = str_msg.substr(0, loc);
        str_msg = str_msg.substr(loc+1);
        local_pos = getLocalPosition(i);
        
        if (local_pos == MAIN_USER || str_userphoto.empty() == true) continue;
        
        gamePlayers[local_pos].photo_id = str_userphoto;
    }
    //get player levels
    for (int i = 0; i < MAX_PLAYERS; i++) {
        
        loc = str_msg.find(',');
        std::string str_userlevel = str_msg.substr(0, loc);
        str_msg = str_msg.substr(loc+1);
        local_pos = getLocalPosition(i);
        
        if (local_pos == MAIN_USER || str_userlevel.empty() == true) continue;
        
        gamePlayers[local_pos].level = atoi(str_userlevel.c_str());
    }
    //get player points
    for (int i = 0; i < MAX_PLAYERS; i++) {
        
        loc = str_msg.find(',');
        std::string str_userpoint = str_msg.substr(0, loc);
        str_msg = str_msg.substr(loc+1);
        local_pos = getLocalPosition(i);
        
        if (local_pos == MAIN_USER || str_userpoint.empty() == true) continue;
        
        gamePlayers[local_pos].points = atof(str_userpoint.c_str());
    }
//    //get player gifts
//    for (int i = 0; i < MAX_PLAYERS; i++) {
//        
//        loc = str_msg.find(',');
//        std::string str_gift = str_msg.substr(0, loc);
//        str_msg = str_msg.substr(loc+1);
//        local_pos = getLocalPosition(i);
//        
//        if (local_pos == MAIN_USER || str_gift.empty() == true) continue;
//        
//        gamePlayers[local_pos].gift_id = atoi(str_gift.c_str());
//    }
    
    for (int i = 0; i < MAX_PLAYERS; i++) {
     
        if(i == MAIN_USER) continue;
        if(gamePlayers[i].flagEmpty == true) continue;
        
        showPlayer(i);
    }
}

void GameScene_Online::showDealerMark()
{
    for (int i = 0; i < MAX_PLAYERS; i++) {
        
        if (i == dealer) {
            
            img_dealer[i]->setVisible(true);
        }
        else {
            
            img_dealer[i]->setVisible(false);
        }
    }
}

void GameScene_Online::updatePot()
{
    if(game_pot > 0) {
        
        lb_pot->setString(getNormalText2Number(game_pot));
        
        int index = 0;
        if(game_pot > 15000) {
            
            index = 8;
        }
        else if(game_pot > 10000) {
            
            index = 7;
        }
        else if(game_pot > 7000) {
            
            index = 6;
        }
        else if(game_pot > 4000) {
            
            index = 5;
        }
        else if(game_pot > 2000) {
            
            index = 4;
        }
        else if(game_pot > 1000) {
            
            index = 3;
        }
        else if(game_pot > 500) {
            
            index = 2;
        }
        else if(game_pot > 200) {
            
            index = 1;
        }
        img_pot->loadTexture(StringUtils::format("pot_%d.png", index), TextureResType::PLIST);
        img_pot->setVisible(true);
        return;
    }
    
    img_pot->setVisible(false);
}

void GameScene_Online::showGamePlaying(int server_status)
{
    //stopLoadingAnimation();
    
    //init current game status
    if(server_status == SERVER_RESET || server_status == SERVER_RISK) {
        
        gameStatus = STOP_STATUS;
    }
    else if(server_status == SERVER_FINISH) {
        
        exitGame();
        return;
    }
    else if(server_status == SERVER_DEAL) {
        
        gameStatus = DEAL_STATUS;
    }
    else if(server_status == SERVER_SHOW || serverStatus == SERVER_PLAY) {
        
        gameStatus = PLAY_STATUS;
    }
    else if(server_status == SERVER_PLAY_DONE || serverStatus == SERVER_TRICK_DONE) {
        
        gameStatus = CHECK_STATUS;
    }
    
    //show dealer
    if (gameStatus != STOP_STATUS) {
    
        showDealerMark();
        updatePot();
    }
    
    //show table cards
    for (int i = 0; i < MAX_PLAYERS; i++) {
        
        if(gamePlayers[i].flagEmpty == true) continue;
        if(gamePlayers[i].activeStatus == INVALID) continue;
        
        img_hands[i]->setVisible(true);
    }
    
    isOnlyShow = true;
    GameController::getInstance()->endProcessing();
    
    AppService::hideCircleProgressbar();
}

Card GameScene_Online::getCardFromID(int id)
{
    Card card;
    if (id < 0 || id >= NUM_CARDS) {
        
        card.suit = NONE_SUIT;
    }
    else {
        
        card.suit = id/MAX_NUMBER;
        card.number = id%MAX_NUMBER;
    }
    
    return card;
}

void GameScene_Online::showActiveStatus(int pos)
{
    if(gamePlayers[pos].activeStatus == INVALID) {
        
        if(pos == MAIN_USER) {
            
            flagPlaying = false;
            playtime = 0;
        }
        img_photo[pos]->setOpacity(DISABLE_OPACITY);
        //img_photo[pos]->setColor(Color3B::GRAY);
    }
    else {
        
        if(pos == MAIN_USER) {
            
            flagPlaying = true;
        }
        //img_photo[pos]->setColor(Color3B::WHITE);
        img_photo[pos]->setOpacity(ENABLE_OPACITY);
    }
}

void GameScene_Online::showPlayer(int pos)
{
    if(gamePlayers[pos].flagEmpty == true) return;
    
    g_bChatBan[pos] = false;
    
    panelProfile[pos]->setVisible(true);
    img_photo[pos]->setVisible(true);
    img_border[pos]->setVisible(true);
    img_levelbg[pos]->setVisible(true);
    //panelInfo[pos]->setVisible(true);
    
    if (g_nRoomType == MASTER_ROOM) {
    
        img_invite[pos]->setVisible(false);
    }
    
    std::string strName = gamePlayers[pos].name.substr(0, 10);
    getUppercaseString(strName);
    
    lb_name[pos]->setString(strName);
    lb_level[pos]->setString(StringUtils::format("%d", gamePlayers[pos].level+1));
    lb_point[pos]->setString(getNormalText2Number(gamePlayers[pos].points));
    
    if (gamePlayers[pos].gift_id != -1) {
        
        img_gift[pos]->loadTexture(StringUtils::format("gifticon_%d.png", gamePlayers[pos].gift_id), TextureResType::PLIST);
        img_gift[pos]->setVisible(true);
    }
    
    showActiveStatus(pos);
    
    WebService::getInstance()->setProfileImage(gamePlayers[pos].photo_id, img_photo[pos]);
}

void GameScene_Online::removePlayer(int pos)
{
    stopTimerCountdown();
    
    g_bChatBan[pos] = false;
    
    gamePlayers[pos].init();
    img_photo[pos]->setVisible(false);
    img_border[pos]->setVisible(false);
    img_levelbg[pos]->setVisible(false);
    img_hands[pos]->setVisible(false);
    
    img_emoticon[pos]->setVisible(false);
    img_ebg[pos]->setVisible(false);
    //panelInfo[pos]->setVisible(false);
    
    if (g_nRoomType == MASTER_ROOM) {
        img_invite[pos]->setVisible(true);
        panelProfile[pos]->setVisible(true);
        lb_name[pos]->setVisible(false);
        lb_level[pos]->setVisible(false);
        lb_point[pos]->setVisible(false);
    }
    else{
        panelProfile[pos]->setVisible(false);
    }
}

void GameScene_Online::showPurpleCacrd(int pos)
{
    //show card on table
    int pcard_index = gamePlayers[pos].purple_card%10;
    std::string file = StringUtils::format("pcard_%d.png", pcard_index);
    FlipActionEffect(img_hands[pos], file);
    
    //show top card
    file = StringUtils::format("pscard_%d.png", pcard_index);
    img_pcard->loadTexture(file, TextureResType::PLIST);
    img_pcard->runAction(Sequence::create(
                                         DelayTime::create(0.1),
                                         MoveTo::create(0.2f, posPCards[1]),
                                         NULL));
    
    if (g_bSound == true) {
        
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("pcard_flip.aac", false);
    }
}

void GameScene_Online::showGreenCard()
{
    //show card on table
    int gcard_index;
    if(green_card > 39) {
        
        gcard_index = 0;
        
        node_monkey->stopAllActions();
        node_monkey->setVisible(true);
        node_monkey->runAction(pMonkeyAnimation);
        pMonkeyAnimation->setTimeSpeed(0.8);
        pMonkeyAnimation->gotoFrameAndPlay(0, false);
        
//        if (g_bSound == true) {
//
//            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("gcard.aac", false);
//        }
    }
    else {
        
        gcard_index = (green_card%10) + 1;
    }
    
    std::string file = StringUtils::format("gscard_%d.png", gcard_index);
    
    //show top card
    img_gcard->loadTexture("gscard_back.png", TextureResType::PLIST);
    img_gcard->runAction(Sequence::create(
                                          DelayTime::create(0.1),
                                          MoveTo::create(0.2f, posGCards[1]),
                                          CallFunc::create(CC_CALLBACK_0(GameScene_Online::FlipActionEffect, this, img_gcard, file)),
                                          NULL));
}

void GameScene_Online::hideShowCard()
{
    img_pcard->setPosition(posPCards[0]);
    img_gcard->setPosition(posGCards[0]);
}

void GameScene_Online::addNewMessage(std::string name, std::string text)
{
    //btn_chat->setTitleText("");
    lb_title[0]->setVisible(false);
    
    int num = lv_chathistory->getItems().size();
    for (int i = 0;  i < num-4; i++) {
        
        lv_chathistory->removeItem(0);
    }
    
    auto _richText = RichText::create();
    //_richText->ignoreContentAdaptWithSize(false);
    //Size szScrollView = lv_chat->getBoundingBox().size;
    //_richText->setSize(szScrollView);
    
    RichElementText* re1 = RichElementText::create(1, Color3B(179, 107, 201), 255, name+":", "arial", 36);
    RichElementText* re2 = RichElementText::create(2, Color3B::WHITE, 255, text, "arial", 36);
    
    _richText->pushBackElement(re1);
    _richText->pushBackElement(re2);
    _richText->formatText();
    
    LinearLayoutParameter* linearLayoutParameter = (LinearLayoutParameter*)(_richText->getLayoutParameter());
    if (!linearLayoutParameter)
    {
        linearLayoutParameter = LinearLayoutParameter::create();
    }
    linearLayoutParameter->setGravity(LinearLayoutParameter::LinearGravity::LEFT);
    _richText->setLayoutParameter(linearLayoutParameter);
   
    lv_chathistory->pushBackCustomItem(_richText);
    lv_chathistory->jumpToBottom();
    
    lv_chathistory->refreshView();
}

void GameScene_Online::addNewMessage2MainUser(std::string text)
{
    //btn_chat->setTitleText("");
    lb_title[0]->setVisible(false);
    
    int num = lv_chathistory->getItems().size();
    for (int i = 0;  i < num-3; i++) {
        
        lv_chathistory->removeItem(0);
    }
    
    //name
    auto _richName = RichText::create();
    RichElementText* re1 = RichElementText::create(1, Color3B(179, 107, 201), 255, g_strGameScene[1][g_nLangType], "arial", 32);
    
    _richName->pushBackElement(re1);
    _richName->formatText();
    
    LinearLayoutParameter* linearLayoutParameter = (LinearLayoutParameter*)(_richName->getLayoutParameter());
    if (!linearLayoutParameter)
    {
        linearLayoutParameter = LinearLayoutParameter::create();
    }
    
    linearLayoutParameter->setGravity(LinearLayoutParameter::LinearGravity::RIGHT);
    _richName->setLayoutParameter(linearLayoutParameter);
    
    lv_chathistory->pushBackCustomItem(_richName);
    
    //chat text
    auto _richText = RichText::create();
    //_richText->ignoreContentAdaptWithSize(false);
    //Size szScrollView = lv_chat->getBoundingBox().size;
    //_richText->setSize(szScrollView);
    
    RichElementText* re2 = RichElementText::create(2, Color3B::WHITE, 255, text, "arial", 32);
    
    _richText->pushBackElement(re2);
    _richText->formatText();
    
    linearLayoutParameter = (LinearLayoutParameter*)(_richText->getLayoutParameter());
    if (!linearLayoutParameter)
    {
        linearLayoutParameter = LinearLayoutParameter::create();
    }
    
    linearLayoutParameter->setGravity(LinearLayoutParameter::LinearGravity::RIGHT);
    _richText->setLayoutParameter(linearLayoutParameter);
    
    lv_chathistory->pushBackCustomItem(_richText);
    lv_chathistory->jumpToBottom();

    lv_chathistory->refreshView();
}

void GameScene_Online::onChatMessageDone(std::string msg)
{
    pChatLayer->hideLayer();
    sendChattingMsg(msg);
}

void GameScene_Online::sendChattingMsg(std::string msg)
{
    std::string strEncode = base64_encode(reinterpret_cast<const unsigned char*>(msg.c_str()), msg.length());
    
    std::string chat_msg = "chatting:" +  strEncode;
    GameController::getInstance()->sendChatMessage(chat_msg);
    showUserMsg(MAIN_USER, msg);
}

void GameScene_Online::showUserMsg(int player_id, std::string msg)
{
    if(player_id == INVALID) return;
    
    if(player_id == MAIN_USER) {
        
        addNewMessage2MainUser(msg);
    }
    else {
        
        addNewMessage(gamePlayers[player_id].name, msg);
    }
    std::vector<std::string> vMsg = getNormalMessages(msg);
    for (int i = 0; i < vMsg.size(); i++) {
        
        if(player_id == MAIN_USER) {
            
            //addNewMessage2MainUser(vMsg[i]);
        }
        else {
            
            //addNewMessage(gamePlayers[player_id].name, vMsg[i]);
            pChatLayer->addNewMessage(gamePlayers[player_id].name, vMsg[i]);
        }
    }
    
    if (vMsg.size() == 1) {
    
        lb_chat[player_id]->setString(vMsg[0]);
        
        int wid = lb_chat[player_id]->getContentSize().width;
        
        Size szBubble = img_chat[player_id]->getContentSize();
        float scaleW = (wid+50)/(float)szBubble.width;
        //lb_chat[player_id]->setPosition(Vec2(szBubble.width/2.0f, szBubble.height/2.0f));
        
        lb_chat[player_id]->setScaleX(1/scaleW);
        img_chat[player_id]->setScaleX(scaleW);
        //img_chat[player_id]->cocos2d::Node::setContentSize(szBubble);
        img_chat[player_id]->setVisible(true);
    }
    else {
        
        lb_chat2[player_id][0]->setString(vMsg[0]);
        lb_chat2[player_id][1]->setString(vMsg[1]);
        
        int wid1 = lb_chat2[player_id][0]->getContentSize().width;
        int wid2 = lb_chat2[player_id][1]->getContentSize().width;
        
        int wid = MAX(wid1, wid2);
        
        Size szBubble = img_chat2[player_id]->getContentSize();
        float scaleW = (wid+50)/(float)szBubble.width;
        //szBubble.width = wid+20;
        //lb_chat2[player_id][0]->setPosition(Vec2(szBubble.width/2.0f, szBubble.height/2.0f));
        //lb_chat2[player_id][1]->setPosition(Vec2(szBubble.width/2.0f, szBubble.height/2.0f));
        
        lb_chat2[player_id][0]->setScaleX(1/scaleW);
        lb_chat2[player_id][1]->setScaleX(1/scaleW);
        //img_chat2[player_id]->cocos2d::Node::setContentSize(szBubble);
        img_chat2[player_id]->setScaleX(scaleW);
        img_chat2[player_id]->setVisible(true);
    }
    
    DelayTime *delayTime = DelayTime::create(2);
    CallFunc *call_func = CallFunc::create(CC_CALLBACK_0(GameScene_Online::hideUserMsg, this, player_id));
    Sequence *delayAct = Sequence::create(delayTime, call_func, NULL);
    this->runAction(delayAct);
}

void GameScene_Online::hideUserMsg(int player_id)
{
    img_chat[player_id]->setVisible(false);
    img_chat2[player_id]->setVisible(false);
}

void GameScene_Online::onEmoticonSelectDone(int pos, int index)
{
    int receiver_pos = pos;
    
    //if main user is turn, can't send gift
//    if (timer_index == MAIN_USER) {
//        
//        pAlertLayer->showLayer("", "You can't send emoticon\n during your turn");
//        return;
//    }
    
    if (pos == -1) { //for table
        
        //check points for emoticons, first
        int cnt = 0;
        for (int i = 0; i < MAX_PLAYERS; i++) {
            
            if (gamePlayers[i].flagEmpty == true) continue;
            
            cnt++;
        }
        
        int emoticon_price = cnt*100;
        int restPoint = g_mainUserInfo.points - g_nPlayFee; // - gamePlayers[MAIN_USER].points;
        
//        if (gamePlayers[MAIN_USER].points < g_nPlayFee) {
//            
//            restPoint = g_mainUserInfo.points - g_nBuyin;
//        }
        
        if (restPoint < emoticon_price) {
            
            if (g_nLangType == 0) {
                
                pAlertLayer->showLayer("", "You don't have enough point");
            }
            else {
                
                pAlertLayer->showLayer("", "No tienes suficientes puntos");
            }
            return;
        }
        
        g_mainUserInfo.points -= emoticon_price;
        g_mainUserInfo.total_emoticon += cnt;
        if (g_mainUserInfo.points < gamePlayers[MAIN_USER].points) {
            
            gamePlayers[MAIN_USER].points = g_mainUserInfo.points;
        }
        
        GameController::getInstance()->sendUserInfo();
        
        for (int i = 0; i < MAX_PLAYERS; i++) {
            
            if (gamePlayers[i].flagEmpty == true) continue;
            
            sendEmoticonItem(MAIN_USER, i, index);
        }
        
        WebService::getInstance()->updateEmoticon();
    }
    else {
        
        int emoticon_price = 100;
        int restPoint = g_mainUserInfo.points - g_nPlayFee;//gamePlayers[MAIN_USER].points;

//        if (gamePlayers[MAIN_USER].points < g_nPlayFee) {
//            
//            restPoint = g_mainUserInfo.points - g_nBuyin;
//        }
        
        if (restPoint < emoticon_price) {
            
            if (g_nLangType == 0) {
                
                pAlertLayer->showLayer("", "You don't have enough point");
            }
            else {
                
                pAlertLayer->showLayer("", "No tienes suficientes puntos");
            }
            return;
        }
        
        g_mainUserInfo.points -= emoticon_price;
        g_mainUserInfo.total_emoticon++;
        if (g_mainUserInfo.points < gamePlayers[MAIN_USER].points) {
            
            gamePlayers[MAIN_USER].points = g_mainUserInfo.points;
        }
        
        GameController::getInstance()->sendUserInfo();
        
        sendEmoticonItem(MAIN_USER, pos, index);
        
        WebService::getInstance()->updateEmoticon();
    }
    
    if (g_bSound == true) {
        
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(StringUtils::format("avatar_%d.aac", index/12).c_str(), false);
    }
    
    if (receiver_pos != -1) {
        
        receiver_pos = (server_position+pos)%MAX_PLAYERS;
    }
    std::string strMsg = StringUtils::format("emoticon:%d,%d,%d,%.0f", server_position, receiver_pos, index, gamePlayers[MAIN_USER].points);
    GameController::getInstance()->sendChatMessage(strMsg);
}

void GameScene_Online::onGiftSelectDone(int pos, int index)
{
    if (pos == -1) { //for table
     
        std::string strMsg = StringUtils::format("gift:-1,%d", index);
        GameController::getInstance()->sendChatMessage(strMsg);
        
        for (int i = 0; i < MAX_PLAYERS; i++) {
            
            if (gamePlayers[i].flagEmpty == true) continue;
            
            sendGiftItem(MAIN_USER, i, index);
        }
    }
    else {
     
        std::string strMsg = StringUtils::format("gift:%s,%d", gamePlayers[pos].user_id.c_str(), index);
        GameController::getInstance()->sendChatMessage(strMsg);
        
        sendGiftItem(MAIN_USER, pos, index);
    }
}

void GameScene_Online::onEmoticonMessage(std::string str_msg)
{
    CCLOG("USER_EMOTICON: %s", str_msg.c_str());
    unsigned long loc = str_msg.find(',');
    std::string str_sender = str_msg.substr(0, loc);
    str_msg = str_msg.substr(loc+1);
    
    loc = str_msg.find(',');
    std::string str_receiver = str_msg.substr(0, loc);
    str_msg = str_msg.substr(loc+1);
    
    loc = str_msg.find(',');
    std::string str_emoticon = str_msg.substr(0, loc);
    std::string str_point = str_msg.substr(loc+1);
    
    int sender = std::atoi(str_sender.c_str());
    int sender_pos = getLocalPosition(sender);
    
    float tmp_point = atof(str_point.c_str());
    
    int receiver = std::atoi(str_receiver.c_str());
    int receiver_pos;
    if (receiver != -1) {
    
        receiver_pos = getLocalPosition(receiver);
    }
    else {
        
        receiver_pos = receiver;
    }
    
    int emoticon_id = std::atoi(str_emoticon.c_str());
    
    if (sender_pos != MAIN_USER) {
        
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(StringUtils::format("avatar_%d.aac", emoticon_id/12).c_str(), false);
        }
        //update game points
        gamePlayers[sender_pos].points = tmp_point;
        lb_point[sender_pos]->setString(getNormalText2Number(tmp_point));
        if (receiver_pos == -1) { //for table
            
            for (int i = 0; i < MAX_PLAYERS; i++) {
                
                if (gamePlayers[i].flagEmpty == true) continue;
                
                sendEmoticonItem(sender_pos, i, emoticon_id);
            }
        }
        else {
            
            sendEmoticonItem(sender_pos, receiver_pos, emoticon_id);
        }
    }
}

void GameScene_Online::onUpdateInfoMessage(std::string str_msg)
{
    CCLOG("USER_PHOTO: %s", str_msg.c_str());
    
    unsigned long loc = str_msg.find(':');
    std::string str_sender = str_msg.substr(0, loc);
    str_msg = str_msg.substr(loc+1);
    
    loc = str_msg.find(',');
    std::string str_cash = str_msg.substr(0, loc);
    std::string str_photo = str_msg.substr(loc+1);
    
    int sender = std::atoi(str_sender.c_str());
    int sender_pos = getLocalPosition(sender);
    
    if (sender_pos != MAIN_USER && gamePlayers[sender_pos].flagEmpty == false) {
        
        gamePlayers[sender_pos].points = atof(str_cash.c_str());
        lb_point[sender_pos]->setString(str_cash);
        
        gamePlayers[sender_pos].photo_id = str_photo;
        WebService::getInstance()->setProfileImage(str_photo, img_photo[sender_pos]);
    }
}

void GameScene_Online::onGiftMessage(std::string str_msg)
{
    CCLOG("USER_ITEM: %s", str_msg.c_str());
    unsigned long loc = str_msg.find(':');
    std::string str_sender = str_msg.substr(0, loc);
    str_msg = str_msg.substr(loc+1);
    int sender = std::atoi(str_sender.c_str());
    int sender_pos = getLocalPosition(sender);
    
    if (sender_pos != MAIN_USER) {
        
        //get present item for players
        for (int i = 0; i < MAX_PLAYERS; i++) {
            
            loc = str_msg.find(',');
            std::string str_item = str_msg.substr(0, loc);
            str_msg = str_msg.substr(loc+1);
            int local_pos = getLocalPosition(i);
            
            int item = atoi(str_item.c_str());
            if(item == -1) continue;
            
            DelayTime *delayTime = DelayTime::create(0.1);
            CallFunc *call_func = CallFunc::create(CC_CALLBACK_0(GameScene_Online::sendGiftItem, this, sender_pos, local_pos, item));
            Sequence *delayAct = Sequence::create(delayTime, call_func, NULL);
            this->runAction(delayAct);
        }
    }
}

void GameScene_Online::onUserMessage(std::string user_msg)
{
    std::size_t loc = user_msg.find(':');
    std::string str_pos = user_msg.substr(0, loc);
    std::string str_msg = user_msg.substr(loc+1);
    
    std::string strDecode = base64_decode(str_msg);
    
    int pos = std::atoi(str_pos.c_str());
    int local_pos = getLocalPosition(pos);
    
    if (isOnlyShow == false && local_pos == MAIN_USER) return;
    
    if (g_bChatBan[local_pos] == true) {
        
        return;
    }
    
    DelayTime *delayTime = DelayTime::create(0.1);
    CallFunc *call_func = CallFunc::create(CC_CALLBACK_0(GameScene_Online::showUserMsg, this, local_pos, strDecode));
    Sequence *delayAct = Sequence::create(delayTime, call_func, NULL);
    this->runAction(delayAct);
}

void GameScene_Online::onChatReceived(std::string msg)
{
    if(exitFlag == true || isEnablePlay == false){
        
        GameController::getInstance()->endProcessing();
        return;
    }
    std::size_t loc = msg.find('#');
    std::string str_code = msg.substr(0, loc);
    std::string str_msg = msg.substr(loc+1);
    
    int val = std::atoi(str_code.c_str());
    int pos, local_pos;
    
    if(val == RESULT_GAME_START) {
        
        //initUserInfo(str_msg);
        
        GameController::getInstance()->endProcessing();
        //startGame();
    }
    else if (val == RESULT_USER_JOIN) {
        
        GameController::getInstance()->endProcessing();
        
        loc = str_msg.find(':');
        std::string str_pos = str_msg.substr(0, loc);
        str_msg = str_msg.substr(loc+1);
        
        pos = atoi(str_pos.c_str());
        if(pos == server_position) return;
        local_pos = getLocalPosition(pos);
        
        loc = str_msg.find(',');
        std::string str_userid = str_msg.substr(0, loc);
        str_msg = str_msg.substr(loc+1);
        
        loc = str_msg.find(',');
        std::string str_name = str_msg.substr(0, loc);
        str_msg = str_msg.substr(loc+1);
        
        loc = str_msg.find(',');
        std::string str_photo = str_msg.substr(0, loc);
        str_msg = str_msg.substr(loc+1);
        
        loc = str_msg.find(',');
        std::string str_level = str_msg.substr(0, loc);
        std::string str_points = str_msg.substr(loc+1);
        
        gamePlayers[local_pos].flagEmpty = false;
        gamePlayers[local_pos].user_id = str_userid;
        gamePlayers[local_pos].name = str_name;
        gamePlayers[local_pos].photo_id = str_photo;
        gamePlayers[local_pos].level = atoi(str_level.c_str());
        gamePlayers[local_pos].points = atof(str_points.c_str());
        gamePlayers[local_pos].activeStatus = INVALID;
        
        showPlayer(local_pos);
        
        if(g_nRoomType == MASTER_ROOM) {
            
//            WebService::getInstance()->setProfileImage(str_photo, img_teamPhoto[local_pos]);
//            lb_teamName[local_pos]->setString(str_name);
//            btn_Invite[local_pos]->setVisible(false);
//        
//            player_num++;
//            if (player_num == MAX_PLAYERS) {
//                
//                panelStart->setVisible(true);
//            }
        }
    }
    else if (val == RESULT_USER_LEFT) {
        
        GameController::getInstance()->endProcessing();
        
        int pos = atoi(str_msg.c_str());
        int local_pos = getLocalPosition(pos);
        
        removePlayer(local_pos);
        
//        std::string msg = StringUtils::format("%s left table !", gamePlayers[local_pos].name.c_str());       
//        cocos2d::MessageBox(msg.c_str(), "Notification");
        CCLOG("RESULT_USER_LEFT");
    }
    else if (val == RESULT_TRICK_START) {
        
        loc = str_msg.find(',');
        std::string str_dealer = str_msg.substr(0, loc);
        str_msg = str_msg.substr(loc+1);
        
        int pos = atoi(str_dealer.c_str());
        dealer = getLocalPosition(pos);
        
        CCLOG("Dealer = %d", dealer);
        
        for (int i = 0; i < MAX_PLAYERS; i++) {
            
            loc = str_msg.find(',');
            std::string str_active = str_msg.substr(0, loc);
            str_msg = str_msg.substr(loc+1);
            
            local_pos = getLocalPosition(i);
            gamePlayers[local_pos].activeStatus = atoi(str_active.c_str());
        }
        for (int i = 0; i < MAX_PLAYERS; i++) {
            
            loc = str_msg.find(',');
            std::string str_point = str_msg.substr(0, loc);
            str_msg = str_msg.substr(loc+1);
            
            local_pos = getLocalPosition(i);
            gamePlayers[local_pos].points = atof(str_point.c_str());
        }
        
        for (int i = 0; i < MAX_PLAYERS; i++) {

            if(gamePlayers[i].flagEmpty == true) continue;
            
            lb_point[i]->setString(getNormalText2Number(gamePlayers[i].points));
            if (i == 0 && gamePlayers[0].points <= 19 && g_mainUserInfo.points <= 19)
            {
                showLessLayer();
            }
            showActiveStatus(i);
        }
        
        gameStatus = BEGIN_STATUS;
        run();
//        DelayTime *delayTime = DelayTime::create(0.1);
//        CallFunc *call_func = CallFunc::create(CC_CALLBACK_0(GameScene_Online::run, this));
//        Sequence *delayAct = Sequence::create(delayTime, call_func, NULL);
//        this->runAction(delayAct);
    }
    else if (val == RESULT_RISK) {
        
        loc = str_msg.find(',');
        std::string str_pot = str_msg.substr(0, loc);
        str_msg = str_msg.substr(loc+1);
        
        game_pot = atof(str_pot.c_str());
        
        for (int i = 0; i < MAX_PLAYERS; i++) {
            
            loc = str_msg.find(',');
            std::string str_points = str_msg.substr(0, loc);
            str_msg = str_msg.substr(loc+1);
            
            local_pos = getLocalPosition(i);
            gamePlayers[local_pos].points = atof(str_points.c_str());
        }
        
        gameStatus = RISK_STATUS;
        run();
//        DelayTime *delayTime = DelayTime::create(0.1);
//        CallFunc *call_func = CallFunc::create(CC_CALLBACK_0(GameScene_Online::run, this));
//        Sequence *delayAct = Sequence::create(delayTime, call_func, NULL);
//        this->runAction(delayAct);
    }
    else if (val == PLAYER_HAND) {
        
        CCLOG("USER_HAND: %s", str_msg.c_str());
        
        gamePlayers[MAIN_USER].purple_card = atoi(str_msg.c_str());

        gameStatus = DEAL_STATUS;
        run();
        //        DelayTime *delayTime = DelayTime::create(0.1);
//        CallFunc *call_func = CallFunc::create(CC_CALLBACK_0(GameScene_Online::actionResetDone, this));
//        Sequence *delayAct = Sequence::create(delayTime, call_func, NULL);
//        this->runAction(delayAct);
    }
    else if (val == RESULT_SHOW) {
        
        loc = str_msg.find(',');
        std::string str_player = str_msg.substr(0, loc);
        std::string str_pcard = str_msg.substr(loc+1);
        
        int pos = atoi(str_player.c_str());
        activePlayer = getLocalPosition(pos);
        
        if(gamePlayers[activePlayer].flagEmpty == true)
        {
            GameController::getInstance()->endProcessing();
            return;
        }
        
        gamePlayers[activePlayer].purple_card = atoi(str_pcard.c_str());
        gameStatus = PLAY_STATUS;
        run();
//        DelayTime *delayTime = DelayTime::create(0.1);
//        CallFunc *call_func = CallFunc::create(CC_CALLBACK_0(GameScene_Online::run, this));
//        Sequence *delayAct = Sequence::create(delayTime, call_func, NULL);
//        this->runAction(delayAct);
    }
    else if (val == RESULT_PLAY) {
        
        //player
        loc = str_msg.find(':');
        std::string str_player = str_msg.substr(0, loc);
        str_msg = str_msg.substr(loc+1);
        
        pos = atoi(str_player.c_str());
        activePlayer = getLocalPosition(pos);
        
        //call type
        loc = str_msg.find(',');
        std::string str_calltype = str_msg.substr(0, loc);
        str_msg = str_msg.substr(loc+1);
        
        gamePlayers[activePlayer].call_type = atoi(str_calltype.c_str());
        
        //betting & purple card
        loc = str_msg.find(',');
        std::string str_betting = str_msg.substr(0, loc);
        std::string str_pcard = str_msg.substr(loc+1);
        
        //if purple card don't show yet, it show, first
        if(img_hands[activePlayer]->isVisible() == false) {
        
            gamePlayers[activePlayer].purple_card = atoi(str_pcard.c_str());
            int index = gamePlayers[activePlayer].purple_card%10;
            std::string file = StringUtils::format("pcard_%d.png", index);
            img_hands[activePlayer]->loadTexture(file, TextureResType::PLIST);
            img_pcard->loadTexture(file, TextureResType::PLIST);
            img_pcard->setPosition(posPCards[1]);
            img_pcard->setVisible(true);
        }
        gamePlayers[activePlayer].betting = atof(str_betting.c_str());
        actionPlayDone();
//        DelayTime *delayTime = DelayTime::create(0.1);
//        CallFunc *call_func = CallFunc::create(CC_CALLBACK_0(GameScene_Online::actionPlayDone, this));
//        Sequence *delayAct = Sequence::create(delayTime, call_func, NULL);
//        this->runAction(delayAct);
    }
    else if (val == RESULT_PLAY_DONE) {
        
        //player
        loc = str_msg.find(':');
        std::string str_player = str_msg.substr(0, loc);
        str_msg = str_msg.substr(loc+1);
        
        pos = atoi(str_player.c_str());
        activePlayer = getLocalPosition(pos);
        
        //green card
        loc = str_msg.find(',');
        std::string str_gcard = str_msg.substr(0, loc);
        str_msg = str_msg.substr(loc+1);
        
        green_card = atoi(str_gcard.c_str());
        
        //win type
        loc = str_msg.find(',');
        std::string str_win = str_msg.substr(0, loc);
        str_msg = str_msg.substr(loc+1);
        
        win_type = atoi(str_win.c_str());
        
        //purple card & betting
        loc = str_msg.find(',');
        std::string str_pcard = str_msg.substr(0, loc);
        std::string str_betting = str_msg.substr(loc+1);
        
        //if purple card don't show yet, it show, first
        if(img_hands[activePlayer]->isVisible() == false) {
            
            gamePlayers[activePlayer].betting = atoi(str_betting.c_str());
            gamePlayers[activePlayer].purple_card = atoi(str_pcard.c_str());
            int index = gamePlayers[activePlayer].purple_card%10;
            std::string file = StringUtils::format("pcard_%d.png", index);
            img_hands[activePlayer]->loadTexture(file, TextureResType::PLIST);
            img_pcard->loadTexture(file, TextureResType::PLIST);
            img_pcard->setPosition(posPCards[1]);
            img_pcard->setVisible(true);
            
            setChipTextureWithPoint(img_chips[activePlayer], gamePlayers[activePlayer].betting);
            img_chips[activePlayer]->setVisible(true);
            lb_chips[activePlayer]->setString(StringUtils::format("%.0f", gamePlayers[activePlayer].betting));
        }
        
        showGreenCard();
        
        DelayTime *delayTime = DelayTime::create(0.5);
        CallFunc *call_func = CallFunc::create(CC_CALLBACK_0(GameScene_Online::actionPlayCompleted, this, win_type));
        Sequence *delayAct = Sequence::create(delayTime, call_func, NULL);
        this->runAction(delayAct);
    }
    else if (val == RESULT_TRICK_DONE) {
        
        //stopTimerCountdown();
        hideActiveMark();
        stopWinLoseAnimation();
        hideShowCard();
        for(int i = 0; i < MAX_PLAYERS; i++) {
            
            img_hands[i]->setVisible(false);
        }
        
        if (gamePlayers[0].points < 20 && g_mainUserInfo.points < 20) //19 point less
        {
            showLessLayer();
        }
        
        GameController::getInstance()->endProcessing();
    }
    else if (val == RESULT_GAME_OVER) {
        
        CCLOG("RESULT_GAME_OVER");
        
        //cocos2d::MessageBox("Game Over", "");
        
        GameController::getInstance()->endProcessing();
        exitGame();
    }
}

void GameScene_Online::onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *pEvent)
{

}

void GameScene_Online::onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *pEvent)
{

}

void GameScene_Online::onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *pEvent)
{
	Touch *touch = touches[0];
	Point location = touch->getLocation();
    
    pChatLayer->hideLayer();
    pPublicProfileLayer->hideLayer();
}

void GameScene_Online::autoPlay()
{
//    //hide active cards
//    for(int i = 0; i < NUM_CARDS; i++) {
//        
//        spriteCards_d[i]->setVisible(false);
//    }
//    
//    Player *curPlayer = &gamePlayers[activePlayer];
//    PLAYING_INFO info = curPlayer->thinkPlay();
//    
//    cur_playing.type = info.type;
//    Card handCard = gamePlayers[MAIN_USER].hand.cards[info.hand_card];
//    int cardImage_index = handCard.suit*MAX_NUMBER + handCard.number;
//    cur_playing.hand_card = cardImage_index;
//    
//    cur_playing.vCommonCards.clear();
//    for (int i = 0; i < info.vCommonCards.size(); i++) {
//        
//        int index = info.vCommonCards[i];
//        Card tableCard = Table::getInstance()->common_cards[index];
//        cardImage_index = tableCard.suit*MAX_NUMBER + tableCard.number;
//        cur_playing.vCommonCards.push_back(cardImage_index);
//    }
//    
//    actionSelection();
//    
//    btn_play->setVisible(false);
//    sendInfo(cur_playing);
}

void GameScene_Online::activeAllLights(bool flag)
{
    for (int i = 0; i < 48; i++) {
        
        img_lights[i]->setVisible(flag);
    }
}

void GameScene_Online::update(float time)
{
    if(exitFlag == true) {
     
        stopTimerCountdown();
        return;
    }
    
    //check emoticons for all players
    for (int i = 0; i < MAX_PLAYERS; i++) {
        
        if (gamePlayers[i].flagEmpty == true || gamePlayers[i].emoticon_id == INVALID) continue;
        
        if(gamePlayers[i].emoticon_timer > 20) {
            
            gamePlayers[i].emoticon_id = INVALID;
            gamePlayers[i].emoticon_timer = 0;
            
            img_ebg[i]->setVisible(false);
            img_emoticon[i]->setVisible(false);
        }
        else {
            
            gamePlayers[i].emoticon_timer += time;
        }
    }
    
    pChatLayer->update(time);
    
    if(flagPlaying == true) {
        
        playtime += time;
    }
    
    if(flagTextAnimation == true) {
        
        if(cntTextTimer == 100) {
            
            lb_point[posTextAnimation]->setString(getNormalText2Number(gamePlayers[posTextAnimation].points));
            initTextAnimation();
        }
        else {
            
            curValue += 2*deltaValue;
            lb_point[posTextAnimation]->setString(getNormalText2Number(curValue));
            cntTextTimer += 2;
        }
    }
    
    if(timer_index == INVALID) return;
    
    //change timer color
    durationTimer += time;
    if (timer_index == MAIN_USER) {
      
        if(durationTimer > TURN_TIME-1) {
            
            stopTimerCountdown();
            
            panelAction->setVisible(false);
            panelBetting->setVisible(false);
            
            if(gamePlayers[MAIN_USER].call_type == INVALID) {
                
                img_hands[MAIN_USER]->setVisible(false);
                hideShowCard();
                gamePlayers[MAIN_USER].call_type = FOLD_PLAY;
            }
            sendInfo();
            
            activeAllLights(true);
        }
        else {
            
            float delta = (TURN_TIME-1)/48.0f;
            for (int i = 48; i > 0; i--) {
                
                if (durationTimer > i*delta) {
                    
                    img_lights[i]->setVisible(false);
                    break;
                }
            }
        }
    }
    else {
        
        if(durationTimer > TURN_TIME*0.8) { //red color
            
            timerProgress[timer_index]->setColor(Color3B::RED);
        }
        else if(durationTimer > TURN_TIME*0.6) { //orange color
            
            timerProgress[timer_index]->setColor(Color3B::ORANGE);
        }
        else if(durationTimer > TURN_TIME*0.4) { //yellow color
            
            timerProgress[timer_index]->setColor(Color3B::YELLOW);
        }
    }
}

