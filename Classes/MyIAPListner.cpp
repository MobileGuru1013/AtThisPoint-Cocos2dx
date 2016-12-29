//
//  MyIAPListner.cpp
//
//

#include "MyIAPListner.h"

#include "common.h"
#include "WebService.h"

#include "SpinLayer.h"
#include "StoreLayer.h"

std::string g_strIAP[3][2] = {{"Notification", "Notificación"},
                              {"You get", "Has obtenido"},
                              {"Points", "Puntos"}
};

MyIAPListner* MyIAPListner::_instance = NULL;

MyIAPListner* MyIAPListner::getInstance()
{
    if (_instance == NULL) {
        
        _instance = new MyIAPListner();
    }
    
    return _instance;
}

void MyIAPListner::onInitialized(bool ok)
{
    CCLOG("IAP Initialize %d", ok);
}

void MyIAPListner::onSuccess(sdkbox::Product const& p)
{
    int value;
    if(p.id == "com.pointgame.buy1") {
       
        value = 15000;
        g_mainUserInfo.points += value;
    }
    else if(p.id == "com.pointgame.buy2") {
        
        value = 40000;
        g_mainUserInfo.points += value;
    }
    else if(p.id == "com.pointgame.buy3") {
        
        value = 100000;
        g_mainUserInfo.points += value;
    }
    else if(p.id == "com.pointgame.buy4") {
        
        value = 150000;
        g_mainUserInfo.points += value;
    }
    else if(p.id == "com.pointgame.buy5") {
        
        value = 450000;
        g_mainUserInfo.points += value;
    }
    else if(p.id == "com.pointgame.buy6") {
        
        value = 1500000;
        g_mainUserInfo.points += value;
    }
    else if(p.id == "com.pointgame.buy7") {
        
        g_nFreeSpin++;
        time(&g_timeLastFreeSpin);
        WebService::getInstance()->updateFreeSpin();
        
//        if (g_pSpinLayer->flagActive == true) {
//            
//            g_pSpinLayer->actionRotateSpin();
//        }
//        else
        {
            if (g_pStoreLayer->isVisible() == true) {
                
                g_pStoreLayer->hideLayer();
            }
            g_pSpinLayer->showLayer();
        }
        
        return;
    }
    
    WebService::getInstance()->updatePoint();    
    cocos2d::MessageBox(StringUtils::format("%s %d %s!", g_strIAP[1][g_nLangType].c_str(), value, g_strIAP[2][g_nLangType].c_str()).c_str(), g_strIAP[0][g_nLangType].c_str());
    
    g_pStoreLayer->actionPurchaseDone();
    setGiftBonusFlag(false);
    
    CCLOG("Purchase success : %s , price = %s", p.name.c_str(), p.price.c_str());
}

void MyIAPListner::onFailure(sdkbox::Product const& p, const std::string &msg)
{
    CCLOG("Purchase fail : %s , %s", p.name.c_str(), msg.c_str());
}

void MyIAPListner::onCanceled(sdkbox::Product const& p)
{
    CCLOG("Purchase cancel : %s , price = %s", p.name.c_str(), p.price.c_str());
}

void MyIAPListner::onRestored(sdkbox::Product const& p)
{
    CCLOG("Purchase restore : %s , price = %s", p.name.c_str(), p.price.c_str());
}

void MyIAPListner::onRestoreComplete(bool ok, const std::string &msg)
{
    
}

void MyIAPListner::onProductRequestSuccess(std::vector<sdkbox::Product> const &products)
{
    CCLOG("ProductRequest surcess");
}

void MyIAPListner::onProductRequestFailure(const std::string &msg)
{
    CCLOG("ProductRequest fail : %s", msg.c_str());
}
