#include "SplashScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

#include "common.h"
#include "HelloWorldScene.h"
#include "TutorialScene.h"
#include "GameScene_Online.h"

#include "AppService.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#include "AppleService.h"

#endif

USING_NS_CC;

// #define DEVELOPER_MODE

using namespace cocostudio::timeline;

Scene* SplashScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = SplashScene::create();

	// add layer as a child to scene
	scene->addChild(layer);
	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool SplashScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	rootNode = CSLoader::createNode("SplashScene.csb");
    rootNode->setVisible(false);
	addChild(rootNode);
    
    float scaleY = 1.0f/g_fScaleWH;
    auto img_bg = (ImageView*)rootNode->getChildByName("bg");
    img_bg->setScaleY(scaleY);
    
    panelLoading = (Layout*)(rootNode->getChildByName("panel_loading"));
    pLoadingbar = (LoadingBar*)(panelLoading->getChildByName("pro_bar"));
    pLoadingbar->setPercent(0);
    panelLoading->setVisible(false);
    
    isLoadingStart = false;
    timer = 0;
    loading_counter = 0;
#ifdef DEVELOPER_MODE
    //startAppleRemoteNotificaton();
    // pVideoPlayer->setVisible(false);
    rootNode->setVisible(true);
    // scheduleUpdate();
    this->schedule(schedule_selector(SplashScene::update), 0.0025f);
#else
    showVideo();
#endif
    
    //scheduleUpdate();

	return true;
}

void SplashScene::showVideo()
{
    Size size = Director::getInstance()->getVisibleSize();
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    auto videoPlayer = cocos2d::experimental::ui::VideoPlayer::create();
    videoPlayer->setPosition(Point(size.width*0.5, size.height*0.5));
    videoPlayer->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    videoPlayer->setContentSize(Size(size.width , size.height));
    this->addChild(videoPlayer);
    videoPlayer->setTag(100);
    
    pVideoPlayer = videoPlayer;
    
    if (videoPlayer)
    {
        videoPlayer->setFileName("logo-intro.mp4");
        //videoPlayer->setFullScreenEnabled(true);
        videoPlayer->play();
    }
    videoPlayer->addEventListener(CC_CALLBACK_2(SplashScene::videoEventCallback, this));
//#endif
}

void SplashScene::videoEventCallback(Ref* sender, cocos2d::experimental::ui::VideoPlayer::EventType eventType)
{
    switch (eventType) {
        case cocos2d::experimental::ui::VideoPlayer::EventType::PLAYING:
            break;
        case cocos2d::experimental::ui::VideoPlayer::EventType::PAUSED:
            break;
        case cocos2d::experimental::ui::VideoPlayer::EventType::STOPPED:
            break;
        case cocos2d::experimental::ui::VideoPlayer::EventType::COMPLETED:
            
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
            
   startAppleRemoteNotificaton();
#endif
            
            pVideoPlayer->setVisible(false);
            rootNode->setVisible(true);
            //pVideoPlayer->removeFromParent();
            
            scheduleUpdate();
            //videoPlayOverCallback();
            break;
        default:
            break;  
    }  
}

void SplashScene::update(float delta)
{
    timer++;
    
    if(isLoadingStart == false) {
        
        if(timer < 1*60) return;
        
        timer = 0;
        isLoadingStart = true;
        panelLoading->setVisible(true);
        
//        Scene *pScene = TransitionFade::create(0.5, HelloWorld::createScene());
//        Director::getInstance()->replaceScene(pScene);
        
        return;
    }
    
    if(timer%10 == 0) {
        
        timer = 0;
        loading_counter++;
        if(loading_counter > g_nLimitLoadingCount) {
            
            loading_counter = 0;
        }
        pLoadingbar->setPercent(loading_counter);
        if(loading_counter == 100) {
            
            if (g_bMusic == true) {
                
                //CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
                CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("lobby.aac", true);
            }
            
            isLoadingStart = false;
            Scene *pScene;
            if (g_mainUserInfo.signup_flag == 0) {
                
                if(g_mainUserInfo.points < g_nFeeList[0] || g_bQplay == false) {
                    pScene = TransitionFade::create(0.5, HelloWorld::createScene());
                }
                else {
                    
                    AppService::showCircleProgressbar();
                    
                    g_nBuyin = g_nBuyinList[0];
                    if (g_mainUserInfo.points < g_nBuyin) {
                        
                        g_nBuyin = g_mainUserInfo.points;
                    }
                    
                    g_nPlayFee = g_nFeeList[0];
                    g_nRoomType = FREE_ROOM;
            
                    pScene = TransitionFade::create(0.5, GameScene_Online::createScene());
                }
            }
            else {
            
                pScene = TransitionFade::create(0.5, TutorialScene::createScene());
            }
            
            Director::getInstance()->replaceScene(pScene);
        }
    }
}
