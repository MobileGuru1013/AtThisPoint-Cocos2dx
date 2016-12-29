
#include "common.h"
#include "AlertLayer.h"

#include "CustomTableLayer.h"

class CarouselLayer : public cocos2d::Layer, public LayerDelegate
{
public:
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	Layer* createLayer(Layer *parent, int zorder);

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	bool init();

	void showLayer();

	// implement the "static create()" method manually
	CREATE_FUNC(CarouselLayer);
    
    void updateImg2Lang();

private:
    
    void onUpdatedDone();

	void onPanelTouchEvent(Ref* pSender, Widget::TouchEventType type);
	void onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id);

	void checkItemSelected(Vec2 pos);

	void arrangeItems();

	float spin_angle;

	int select_index;

	bool flagTouchMoved;

	Layer *pParentLayer;
	Layer *pCurLayer;

	Vec2 prevPos, curPos;
	
	Layout *panelMain;
	ImageView *img_item[5];
    ImageView *img_title[5];
    
    AlertLayer *pAlertLayer;
    CustomTableLayer *pCustomTableLayer;
};
