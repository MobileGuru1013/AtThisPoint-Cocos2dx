#ifndef __ONLINEGAME_SCENE_H__
#define __ONLINEGAME_SCENE_H__

#include "common.h"

#include "Deck.h"
#include "Player.h"
#include "AlertLayer.h"
#include "LayerDelegate.h"
#include "PublicProfileLayer.h"
#include "ChatLayer.h"
#include "LevelupLayer.h"

#include "StoreLayer.h"
#include "SpinLayer.h"
#include "TableInviteLayer.h"
#include "ConnectingLayer.h"

class GameScene_Online : public cocos2d::Layer, public LayerDelegate
{
public:
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene* createScene();

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();

	// implement the "static create()" method manually
	CREATE_FUNC(GameScene_Online);
    
    void onMasterRoomCreated();
    void onRPCMessage(std::string str_msg, int type);
    void onChatReceived(std::string msg);
    void onUserMessage(std::string user_msg);
    void onGiftMessage(std::string user_msg);
    void onEmoticonMessage(std::string user_msg);
    void onUpdateInfoMessage(std::string user_msg);
    
    void showAlert(std::string msg);
    void showErrorMsg(std::string msg);

private:

	void cleanup();
    void update(float delta);

	virtual void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *pEvent);
	virtual void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *pEvent);
	virtual void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *pEvent);

    void onSliderEvent(Ref* sender, Slider::EventType type);
    void onBettingChecked(Ref *sender, Widget::TouchEventType type, int btn_id);
	void onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);
    void onProfileClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);
    void onInviteBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);
    void onFindTeamBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);
    
    void onGetPublicProfileDone(HttpClient* sender, HttpResponse* response, int pos);

    //protect functions for layer delegate
	void onUpdatedDone();
    void onGetFreesSpinDone();
    void onChatMessageDone(std::string msg);
    void onGiftSelectDone(int pos, int index);
    void onEmoticonSelectDone(int pos, int index);

	void endMove(int player_id, int backcard_id, int index, int frontcard_id);
    void endMove_1(int player_id, int backcard_id, int index);
    
	bool checkValidSelection(int card_index);
	void putCard2Pile(int player_id, Card new_card);
    void moveSelectionCards();
    
    void initTimerCountdown(Node *rootNode);
    void runTimerCountdown(int player_id);
    void stopTimerCountdown();

	void actionResetDone();
	void actionDealDone();
	void actionPlayDone();
    void actionPlayCompleted(int res);
	//void actionTrickDone();
	void actionRoundDone();
	void actionDealComplete();
    void actionSelection();
    void actionLeaveGameDone();

	void run();
	void startGame();
	void resetGame();
    void risking();
	void dealing();
	void play();
    void autoPlay();
	//bool scoring();
    
    void exitGame();

    void initUserInfo(std::string msg);
	void showEffectMsg(int type);
	void showActiveMark(int player_id);
	void hideActiveMark();
    void showRoundAlarm();
    
    void showUserMsg(int player_id, std::string msg);
    void hideUserMsg(int player_id);
    void showDealerMark();
    void showGamePlaying(int server_status);
    void updatePot();
    
    int getLocalPosition(int serverPos);
    Card getCardFromID(int id);
    
    void sendInfo();
    void sendChattingMsg(std::string msg);
    
    void sendEmoticonItem(int sender_id, int receiver_id, int item_id);
    void sendGiftItem(int sender_id, int receiver_id, int item_id);
    
    //////////////
    void addNewMessage(std::string name, std::string text);
    void addNewMessage2MainUser(std::string text);
    
    void updatePlayerPointLabel(int pos);
    void updateBettingLabel();
    void updateLevelPoints();
    
    void setChipTextureWithPoint(ImageView *img_chip, float point);
    void showPlayer(int pos);
    void removePlayer(int pos);
    void showActiveStatus(int pos);
    void showPurpleCacrd(int pos);
    void showGreenCard();
    void hideShowCard();
    
    void activeAllLights(bool flag);
    void stopWinLoseAnimation();
    
    void actionWinAnimation(int pos);
    void actionLoseAnimation(int pos);
    void actionAnimation2Pot(int index);
    void actionSetCardTexture(ImageView *imgCard, std::string file);
    void FlipActionEffect(ImageView *imgCard, std::string file);
    
    void initTextAnimation();
    
    void updateString2Lang();
    
    void importLessLayer();
    void showLessLayer();

    int server_position;
    float game_pot;
    float min_betting, max_betting, cur_betting;    
    
    int serverStatus;
	int gameStatus;
	int dealer;
	int activePlayer;
	int lastWinner;

    int cntDealingCards;
	int numCardsInHand;
	int numTricks;
	int numRounds;
    
    //parameter for text animation
    float curValue;
    float deltaValue;
    int cntTextTimer;
    int posTextAnimation;
    bool flagTextAnimation;

	bool isOnlyShow;
	bool isEnablePlay;
    bool exitFlag;
    
    float durationTimer;
    //bool flagTimer;
    int timer_index;
    
    int green_card;
    int win_type;
    
    //parameters for playing data
    float playtime;
    int win_streak;
    bool flagPlaying;
    
    unsigned int timersound_index;
    
    Point posPCards[2];
    Point posGCards[2];

	Deck itsDeck;

	Player gamePlayers[MAX_PLAYERS];
    
    ParticleSystem *particle_cleanpot;
    ParticleSystem *particle_chip[4];
    ParticleSystem *particle_win[4];
    ParticleSystem *particle_lose[2];
    ParticleSystem *particle_lights[23];
    
    ProgressTimer* timerProgress[MAX_PLAYERS];
    Layout *panelProfile[MAX_PLAYERS];
    Layout *panelInfo[MAX_PLAYERS];
    
    Button *btn_sound;
    Button *btn_chat;
    
    ListView *lv_chathistory;
  
    Text *lb_round;
	Text *lb_name[MAX_PLAYERS];
    Text *lb_level[MAX_PLAYERS];
    Text *lb_point[MAX_PLAYERS];
    Text *lb_pot;
    Text *lb_chips[MAX_PLAYERS];
    Text *lb_chat[MAX_PLAYERS];
    Text *lb_chat2[MAX_PLAYERS][2];
    
    Text *lb_title[4];
    
    ImageView *img_winmark;
    ImageView *img_losemark;
    ImageView *img_table;

    ImageView *img_pcard;
    ImageView *img_gcard;
	ImageView *img_backCards[NUM_CARDS];
    
    ImageView *img_ebg[MAX_PLAYERS];
    ImageView *img_emoticon[MAX_PLAYERS];

    ImageView *img_invite[MAX_PLAYERS];
	ImageView *img_photo[MAX_PLAYERS];
    ImageView *img_levelbg[MAX_PLAYERS];
    ImageView *img_gift[MAX_PLAYERS];
    ImageView *img_border[MAX_PLAYERS];
	ImageView *img_active[MAX_PLAYERS];
	ImageView *img_hands[MAX_PLAYERS];
	ImageView *img_deck;
	ImageView *img_pot;
    ImageView *img_chips[MAX_PLAYERS];
    ImageView *img_dealer[MAX_PLAYERS];
    ImageView *img_chat[MAX_PLAYERS];
    ImageView *img_chat2[MAX_PLAYERS];
    
    ImageView *img_lights[48];
    
    ImageView *imgAnimationGifts[MAX_PLAYERS];
    ImageView *imgAnimationChips[MAX_PLAYERS];
    Text *lb_AnimationChips[MAX_PLAYERS];
    
    Layout *panelAction;
    Layout *panelBetting;
    
    Slider *slider_betting;
    Text *lb_betting;
    Button *cb_betting;
    
    Node *node_monkey;
    ActionTimeline *pMonkeyAnimation;

	AlertLayer *pAlertLayer;
    ChatLayer *pChatLayer;
    PublicProfileLayer *pPublicProfileLayer;
    LevelupLayer *pLevelupLayer;
    StoreLayer *pStoreLayer;
    SpinLayer *pSpinLayer;
    TableInviteLayer *pTableInviteLayer;
    ConnectingLayer *pConnectingLayer;
    
    //Less Point Layer
    Node        *pLessPointLayer;
    
//	ResultLayer *pResultLayer;
//	EffectLayer *pEffectLayer;
//    SettingLayer *pSettingLayer;
//    FriendLayer *pFriendLayer;
//    WaitingLayer *pWaitingLayer;
//    RoundAlarmLayer *pRoundAlarmLayer;
    
//    Node *layerFindTeam;
};

#endif // __ONLINEGAME_SCENE_H__
