//
//  TutorialScene.hpp
//  PointGame
//
//  Created by TianXinxing on 5/2/16.
//
//

#ifndef TutorialScene_hpp
#define TutorialScene_hpp

#include "common.h"

#include "Player.h"
#include "RegisterLayer.h"


class TutorialScene : public cocos2d::Layer, public LayerDelegate
{
public:
    
    static cocos2d::Scene* createScene();
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    // implement the "static create()" method manually
    CREATE_FUNC(TutorialScene);
    
protected:
    
    virtual void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *pEvent);
    virtual void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *pEvent);
    virtual void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *pEvent);
    
    void onUpdatedDone();
    void update(float time);
public:
    void updateString();
private:
    
    void onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);
    void onSliderEvent(Ref* sender, Slider::EventType type);
    void onBettingChecked(Ref *sender, Widget::TouchEventType type, int btn_id);
    
    void updatePot();
    void updateBettingLabel();
    void showPlayer();
    void showPurpleCacrd(int pos);
    void showGreenCacrd();
    void showMonkeyCacrd();
    void hideShowCard();
    void showBonusLayer();

    void FlipActionEffect(ImageView *imgCard, std::string file);
    void setChipTextureWithPoint(ImageView *img_chip, float point);
    void actionAnimation2Pot(int index);
    void actionSetCardTexture(ImageView *imgCard, std::string file);
    void actionPlayDone();
    void actionPlayCompleted();
    void actionWinAnimation();
    
    void initTextAnimation();
    
    void risking();
    void dealing();
    
    void run();
    
    void showFocusParticle(int index);
    
    
    Player gamePlayers[MAX_PLAYERS];
    
    RegisterLayer *pRegisterLayer;
    
    Node *layerBonus;
    Node *node_effect;
    
    Layout *panelAction;
    Layout *panelBetting;
    Layout *panelProfile;
    
    ImageView *img_photo;
    ImageView *img_pcard;
    ImageView *img_gcard;
    ImageView *img_hands;
    ImageView *img_chips;
    ImageView *img_deck;
    ImageView *img_backCards;
    ImageView *img_pot;
    ImageView *img_winmark;
    
    ImageView *imgAnimationChips;
    Text *lb_AnimationChips;
    
    ImageView *img_bubble[2];
    Text *lb_bubbles[15];
    Text *lb_imgbubble[2];
    
    ImageView *img_monkey[2];
    ImageView *img_monkeybubble[2];
    
    Text *lb_name;
    Text *lb_level;
    Text *lb_point;
    Text *lb_chips;
    Text *lb_pot;
    Text *lb_betting;
    
    Text *lb_title[3];
    
    Button *btn_action[2];
    
    Text* lbl_skip;
    Text* lbl_fold;
    Text* lbl_bet;
    
    
    Slider *slider_betting;
    
    ParticleSystem *particle_chip[4];
    ParticleSystem *particle_win[4];
    ParticleSystem *particle_focus[5];
    
    ActionTimeline *pNodeAnimation;
    
    Vec2 posPCards[2];
    Vec2 posGCards[2];
    
    int min_betting, max_betting;
    int cur_betting;
    int game_pot;
    
    int cur_step;
    
    bool isNewRegister;
    
    //parameter for text animation
    float curValue;
    float deltaValue;
    int cntTextTimer;
    int posTextAnimation;
    bool flagTextAnimation;
    
    Button* btn_bonus_playnow;
    Text* lbl_bonus_prize;
    Text* lbl_bonus_title;
    
    bool isTouchEnable = true;
};
extern TutorialScene* g_TutorialLayer;

#endif /* TutorialScene_hpp */
