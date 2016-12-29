
#include "AlertLayer.h"

Layer* AlertLayer::createLayer(Layer *parent, int zorder, LayerDelegate *delegate)
{
	pParentLayer = parent;
    pDelegate = delegate;
	// 'layer' is an autorelease object
	pCurLayer = Layer::create();
	pCurLayer->setVisible(false);

	// add layer as a child to parent
	pParentLayer->addChild(pCurLayer, zorder);

	init();

	// return the scene
	return pCurLayer;
}

bool AlertLayer::init()
{    //////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	auto rootNode = CSLoader::createNode("AlertLayer.csb");
	//rootNode->setScale(getScaleValX(), getScaleValY());
	pCurLayer->addChild(rootNode);
    
//    auto mainPanel = static_cast<Layout*>(rootNode->getChildByName("main_panel"));
//    mainPanel->setScaleX(g_fScaleWH);
    for (int i = 0; i < 1; i++) {
        
        auto btnAlert = static_cast<Button*>(rootNode->getChildByName(StringUtils::format("btn_%d", i)));
        btnAlert->addTouchEventListener(CC_CALLBACK_2(AlertLayer::onMainBtnClicked, this, i));
        
        btnAlert->setScaleX(g_fScaleWH);
    }

	lb_title = static_cast<Text*>(rootNode->getChildByName("title"));
    lb_msg = static_cast<Text*>(rootNode->getChildByName("msg"));
	
	return true;
}

void AlertLayer::onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
	if (type == Widget::TouchEventType::ENDED) {

		if (btn_id == 0) { //ok button

            if (g_bSound == true) {
                
                CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
            }
            
            pDelegate->child_index = ALERT_LAYER;
            pDelegate->onUpdatedDone();
		}        
        hideLayer();
	}
}

void AlertLayer::showLayer(std::string title, std::string msg)
{
    lb_title->setString(title);
	lb_msg->setString(msg);
	pCurLayer->setVisible(true);
}

void AlertLayer::hideLayer()
{
	pCurLayer->setVisible(false);
}

