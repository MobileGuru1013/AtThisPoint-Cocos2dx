//
//  SpinLayer.hpp
//  PointGame
//
//  Created by TianXinxing on 4/29/16.
//
//

#ifndef SpinLayer_hpp
#define SpinLayer_hpp

#include "common.h"
#include "LayerDelegate.h"

class SpinLayer : public cocos2d::Layer
{
public:
    
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    Layer* createLayer(Layer *parent, int zorder, LayerDelegate *delegate);
    
    //    Layer* getLayer(){ return pCurLayer; };
    void showLayer();
    void hideLayer();
    
    // implement the "static create()" method manually
    CREATE_FUNC(SpinLayer);
    
    void actionRotateSpin();
    void updateString2Lang();
    
    bool flagActive = false;
    
protected:
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    bool init();
    
private:
    
    void onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);
    void onEmoticonBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);
    
    void onAnimationEvent(Frame *frame);
    
    void showSpin();
    void rotateSpin();
    void showWin();
    void showLose();
    
    void startMaskAnimation();

    void actionMaskAnimationDone(int index);
    void actionSpinStopDone();
    void actionMaskAnimation();
    
    LayerDelegate *pDelegate;
    Layer *pParentLayer;
    Layer *pCurLayer;
    
    Layout *panelFrame;
    Layout *panelCardBg;
    Layout *panelText[2];
    
    Button *btn_spin;
    
    ImageView *img_chipbg;
    ImageView *img_bottomMask;
    ImageView *img_spin;
    ImageView *img_light;
    
    Node *node_effect;
    
    CCSize sznMask[12];
    
    Layout *panel_mask[12];
    Layout *panel_onlight;
    
    Text *lb_spintitle[2];
    Text *lb_winpoint;
    
    Text *lb_title[7];
    
    ParticleSystem *particle_lights[16];
    ParticleSystem *particle_win[2];
    ParticleSystem *particle_sliding;

    ActionTimeline *pAnimation;
    ActionTimeline *pNodeAnimation;
    
    Vec2 posSpinOrigin, posSpinCenter;
    
    int cntMask;
    
    int spin_type;
    int cur_status;
    
    int select_index;
    
    enum ACTION_STATUS
    {
        LAYER_START,
        SPIN_START,
        BUY_SPIN
    };
    
};

#endif /* SpinLayer_hpp */
