#include "AppDelegate.h"
#include "SplashScene.h"

#include "common.h"
#include "WebService.h"
#include "AppService.h"

#include "PluginIAP/PluginIAP.h"
#include "MyIAPListner.h"

USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages, 
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLViewImpl::create("My Game");
        director->setOpenGLView(glview);
    }

    Size frameSize = glview->getFrameSize();
    float ratio = frameSize.width/(float)frameSize.height;
    
    g_fScaleWH = ratio*768/1024.0;
    g_fScaleWH = 1/g_fScaleWH;
    
    director->getOpenGLView()->setDesignResolutionSize(2048, 1536, ResolutionPolicy::EXACT_FIT);
    
    FileUtils::getInstance()->addSearchPath("fonts");
    FileUtils::getInstance()->addSearchPath("Bg");
    FileUtils::getInstance()->addSearchPath("Layers");
    FileUtils::getInstance()->addSearchPath("Plists");
    FileUtils::getInstance()->addSearchPath("Particle");
    FileUtils::getInstance()->addSearchPath("Video");
    FileUtils::getInstance()->addSearchPath(FileUtils::getInstance()->getWritablePath());
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("AvatarPlist.plist", "AvatarPlist.png");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ButtonPlist.plist", "ButtonPlist.png");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("CardPlist.plist", "CardPlist.png");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("ImagePlist.plist", "ImagePlist.png");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("SpinPlist.plist", "SpinPlist.png");
    
    sdkbox::IAP::setListener(MyIAPListner::getInstance());
    sdkbox::IAP::init();
    sdkbox::IAP::setDebug(true);
    
    //init friends
    //	for (int i = 0; i < 25; i++) {
    //
    //		FRIEND_INFO info;
    //		info.user_name = StringUtils::format("Friend_%d", i);
    //		info.photo_id = StringUtils::format("t_%02d", i);
    //
    //		g_vFriends.push_back(info);
    //	}
    
    // turn on display FPS
    director->setDisplayStats(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    register_all_packages();
    
    std::string str_id = cocos2d::UserDefault::getInstance()->getStringForKey(ID_KEY);
    if (str_id.empty() == true)
    {
        cocos2d::UserDefault::getInstance()->setBoolForKey(SOUND, g_bSound);
        cocos2d::UserDefault::getInstance()->setBoolForKey(MUSIC, g_bMusic);
        cocos2d::UserDefault::getInstance()->setBoolForKey(QPLAY, g_bQplay);
        cocos2d::UserDefault::getInstance()->setIntegerForKey(LANG, 0);
    }
    else {
        
        //read settings
        g_bSound = cocos2d::UserDefault::getInstance()->getBoolForKey(SOUND);
        g_bMusic = cocos2d::UserDefault::getInstance()->getBoolForKey(MUSIC);
        g_bQplay = cocos2d::UserDefault::getInstance()->getBoolForKey(QPLAY);
        g_nLangType = cocos2d::UserDefault::getInstance()->getIntegerForKey(LANG);
        
        // AppService::startFreeSpinNotification();
    }
    
    std::string strFB_ID = cocos2d::UserDefault::getInstance()->getStringForKey(FBID_KEY);
    if (strFB_ID.empty() == true) {
    
        WebService::getInstance()->login2Guest();
    }
    else {
        tmp_prev_isFB = true;
        WebService::getInstance()->login2FB(strFB_ID);
    }
    
    g_nLimitLoadingCount = 30;
 
    // create a scene. it's an autorelease object
    auto scene = SplashScene::createScene();
    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
