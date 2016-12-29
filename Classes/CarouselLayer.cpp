
#include "CarouselLayer.h"

#include "GameScene_Online.h"
#include "AppService.h"

std::string g_imgCarousel[5][2] = {{"text_0.png", "_text_0.png"},
    {"text_1.png", "_text_1.png"},
    {"text_2.png", "_text_2.png"},
    {"text_3.png", "_text_3.png"},
    {"text_4.png", "_text_4.png"}
};

Layer* CarouselLayer::createLayer(Layer *parent, int zorder)
{
	pParentLayer = parent;
	
	// 'layer' is an autorelease object
	pCurLayer = Layer::create();
	pCurLayer->setVisible(false);

	// add layer as a child to parent
	pParentLayer->addChild(pCurLayer, zorder);

	init();

	// return the scene
	return pCurLayer;
}

bool CarouselLayer::init()
{    //////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	auto rootNode = CSLoader::createNode("CarouselLayer.csb");
	pCurLayer->addChild(rootNode);

	//title panel
	panelMain = static_cast<Layout*>(rootNode->getChildByName("panel"));
	panelMain->setScaleX(g_fScaleWH);
	panelMain->addTouchEventListener(CC_CALLBACK_2(CarouselLayer::onPanelTouchEvent, this));

	for (int i = 0; i < 5; i++) {

		img_item[i] = static_cast<ImageView*>(panelMain->getChildByName(StringUtils::format("img_%d", i)));
		//img_item[i]->setTouchEnabled(true);
		img_item[i]->addTouchEventListener(CC_CALLBACK_2(CarouselLayer::onMainBtnClicked, this, i));
        
        img_title[i] = (ImageView*)img_item[i]->getChildByName("title");
	}
    
    //init alert layer
    pAlertLayer = new AlertLayer();
    pAlertLayer->createLayer(pCurLayer, 10, this);
    
    //init custom table layer
    pCustomTableLayer = new CustomTableLayer();
    pCustomTableLayer->createLayer(pCurLayer, 5, this);
		
	return true;
}

void CarouselLayer::updateImg2Lang()
{
    for (int i = 0; i < 5; i++) {
        
        img_title[i]->loadTexture(g_imgCarousel[i][g_nLangType], TextureResType::PLIST);
    }
}

void CarouselLayer::onUpdatedDone()
{
    if (child_index == CUSTOMTABLE_LAYER) {
        
        if(g_mainUserInfo.points < g_nPlayFee) {
            
            if (g_nLangType == 0) {
                
                pAlertLayer->showLayer("", "You don't have enough point");
            }
            else {
                
                pAlertLayer->showLayer("", "No tienes suficientes puntos");
            }
            //cocos2d::MessageBox("Yofiru have not enough points", "");
            return;
        }

        if(g_mainUserInfo.points < g_nBuyin) {
            
            g_nBuyin = g_mainUserInfo.points;
        }
        
        AppService::showCircleProgressbar();
        
        g_nRoomType = MASTER_ROOM;
        
        Scene *pScene = TransitionFade::create(0.5, GameScene_Online::createScene());
        Director::getInstance()->replaceScene(pScene);
    }
}

void CarouselLayer::checkItemSelected(Vec2 pos)
{
	//check select index, first
	Rect rect = img_item[select_index]->getBoundingBox();
	Point touchPoint = panelMain->convertToNodeSpace(pos);

	if (rect.containsPoint(touchPoint) == true) {

		CCLOG("Selectd %d", select_index);
        
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        
        if (select_index < 4) { //general tables
            
            int index = select_index;
            
            g_nPlayFee = g_nFeeList[index];
            if(g_mainUserInfo.points < g_nPlayFee) {
                
                if (g_nLangType == 0) {
                    
                    pAlertLayer->showLayer("", "You don't have enough point");
                }
                else {
                    
                    pAlertLayer->showLayer("", "No tienes suficientes puntos");
                }
                return;
            }
            /*
            int nBuyValue = g_nBuyinList[index];
            if (index == 3)
                nBuyValue = 10000;
            
            if(g_mainUserInfo.points < nBuyValue) {
                
                if (g_nLangType == 0) {
                    
                    pAlertLayer->showLayer("", "You don't have enough point");
                }
                else {
                    
                    pAlertLayer->showLayer("", "No tienes suficientes puntos");
                }
                return;
            }
            */
            g_nBuyin = g_nBuyinList[index];
            
            tmp_roomBuyin_key = g_nBuyin;
            if (tmp_roomBuyin_key == -1)
                tmp_roomBuyin_key = 10000;
            
            if(g_nBuyin < 0 || g_mainUserInfo.points < g_nBuyin) {
                g_nBuyin = g_mainUserInfo.points;
            }
            
            
            AppService::showCircleProgressbar();
            
            g_nRoomType = FREE_ROOM;
            Scene *pScene = TransitionFade::create(0.5, GameScene_Online::createScene());
            Director::getInstance()->replaceScene(pScene);
        }
        else { //custom table
            
            pCustomTableLayer->showLayer();
        }

		return;
	}

	for (int i = 0; i < 5; i++) {

		rect = img_item[i]->getBoundingBox();
		touchPoint = panelMain->convertToNodeSpace(pos);

		if (rect.containsPoint(touchPoint) == true) {

			CCLOG("Clicked %d", i);

			int cnt = 10;
			float theta = 360.0f / cnt;
			if (img_item[i]->getPosition().x > 1024) {

				spin_angle += theta;
			}
			else {

				spin_angle -= theta;
			}

			arrangeItems();

			return;
		}
	}
}

void CarouselLayer::onPanelTouchEvent(Ref* pSender, Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::ENDED) {

		if (flagTouchMoved == true) {

			int cnt = 10;
			float theta = 360.0f / cnt;
			float angleToMove = 360.0f - theta * select_index;

			// 逆回転防止
			if (this->spin_angle > 180.0f) this->spin_angle -= 360.0f;
			if (this->spin_angle < -180.0f) this->spin_angle += 360.0f;
			if (angleToMove - this->spin_angle > 180.0f) angleToMove -= 360.0f;
			if (angleToMove - this->spin_angle < -180.0f) angleToMove += 360.0f;

			this->spin_angle = angleToMove;
			this->arrangeItems();

            if (g_bSound == true) {
                
                CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("carrosel.aac", false);
            }            
		}
		else {

			Widget *pWidget = (Widget*)pSender;
			curPos = pWidget->getTouchEndPosition();
			checkItemSelected(curPos);
		}

		flagTouchMoved = false;
			
			//auto moveToHome = [=](float)
			//{are
			//	if (fabs(angleToMove - this->spin_angle) <= 0.1f)
			//	{
			//		pCurLayer->unschedule("moveToHome");
			//		this->spin_angle = angleToMove;
			//		this->arrangeItems();
			//		return;
			//	}

			//	float delta = (angleToMove - this->spin_angle) * 0.1f;
			//	this->spin_angle += delta;
			//	this->arrange0.5Items();
			//};
			//this->schedule(moveToHome, 0.0f, "moveToHome");
	}
	else if (type == Widget::TouchEventType::MOVED) {

		Widget *pWidget = (Widget*)pSender;
		curPos = pWidget->getTouchMovePosition();
        
        int thPixel;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        thPixel = 10;
#endif
        
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        thPixel = 10;
#endif

		float delta = (curPos.x - prevPos.x)/thPixel;//touch->getLocation().x - touch->getPreviousLocation().x;
        if (fabs(delta) > 1) {
            
            flagTouchMoved = true;
            
            spin_angle += delta;
            arrangeItems();
        }
        
        prevPos = curPos;
	}
	else if (type == Widget::TouchEventType::BEGAN) {

		flagTouchMoved = false;

		Widget *pWidget = (Widget*)pSender;
		prevPos = pWidget->getTouchBeganPosition();		
	}
}

void CarouselLayer::onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
	if (type == Widget::TouchEventType::ENDED) {

		if (btn_id == 0) { //close

			CCLOG("%d selected !", btn_id);
		}
	}
}

void CarouselLayer::showLayer()
{
	spin_angle = 0;
	arrangeItems();

    updateImg2Lang();
	pCurLayer->setVisible(true);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
#define PI 3.14159265359f
#define RADIUS 850
#define FLATTEN_RATE 0.01f

void CarouselLayer::arrangeItems()
{
	int cnt = 10;
	float theta = 360.0f / cnt;
	float baseAngle = spin_angle + 270.0f;

	int maxZOrder = -10000;

    float scaleValue[5];
    
	for (int i = 0; i < 5; i++)
	{
		float angle = theta * i + baseAngle;
		float radians = angle * PI / 180.0f;

		float offsetX = cos(radians);
		float offsetY = sin(radians);

		if (offsetY > 0) {

			angle = theta * (i+5) + baseAngle;
			radians = angle * PI / 180.0f;

			offsetX = cos(radians);
			offsetY = sin(radians);
		}		

		float x = RADIUS * offsetX;
		float y = RADIUS * offsetY * FLATTEN_RATE;
		float radiusY = RADIUS * FLATTEN_RATE;
		float diameterY = radiusY * 2;
		float scale = (diameterY - y) / diameterY;
		scale -= 0.5f;
		scale *= scale;
		if (scale < 0.7) {

			scale = 0.7;
		}
        
        scaleValue[i] = scale;

		GLubyte opacity = 255 - (y + radiusY);
		img_item[i]->setPosition(Vec2(1200+x, 300+y));
		//img_item[i]->setScale(scale);
		//img_item[i]->setOpacity(opacity);

		int zorder = diameterY - y;
		if (zorder > maxZOrder) {

			maxZOrder = zorder;
			select_index = i;
		}

		img_item[i]->setZOrder(zorder);
	}

	for (int i = 0; i < 5; i++) {

		if (i == select_index) {

            img_item[i]->setScale(1.2*scaleValue[i]);
			img_item[i]->setColor(Color3B::WHITE);
		}
		else {

            img_item[i]->setScale(scaleValue[i]);
			img_item[i]->setColor(Color3B::GRAY);
		}
	}
}




