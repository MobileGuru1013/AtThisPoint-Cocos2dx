#ifndef __SPLASH_SCENE_H__
#define __SPLASH_SCENE_H__

#include "common.h"

class SplashScene : public cocos2d::Layer
{
public:
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene* createScene();

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();

	// implement the "static create()" method manually
	CREATE_FUNC(SplashScene);
    
    void update(float delta);
    
    void showVideo();
    void videoEventCallback(Ref* sender, cocos2d::experimental::ui::VideoPlayer::EventType eventType);
    
    Layout *panelLoading;
    LoadingBar *pLoadingbar;
    
    Node *rootNode;
    
    bool isLoadingStart;
    int timer;
    int loading_counter = 0;
    
    cocos2d::experimental::ui::VideoPlayer *pVideoPlayer;
};

#endif // __SPLASH_SCENE_H__