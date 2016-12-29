//
//  ConnectingLayer.cpp
//  PointGame
//
//  Created by TianXinxing on 5/26/16.
//
//

#include "ConnectingLayer.h"

//define string
std::string g_strConnecting[2] = {"RECONNECTING...", "RECONECTANDO..."};

Layer* ConnectingLayer::createLayer(Layer *parent, int zorder, LayerDelegate *delegate)
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

bool ConnectingLayer::init()
{    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }
    
    auto rootNode = CSLoader::createNode("ConnectingLayer.csb");
    //rootNode->setScale(getScaleValX(), getScaleValY());
    pCurLayer->addChild(rootNode);
    
    lb_title = (Text*)rootNode->getChildByName("text");
    
    //animation
    pAnimation = CSLoader::createTimeline("ConnectingLayer.csb");
    pAnimation->retain();
    pAnimation->gotoFrameAndPause(0);
    pCurLayer->stopAllActions();
    
    return true;
}

void ConnectingLayer::showLayer()
{
    pCurLayer->stopAllActions();
    pCurLayer->runAction(pAnimation);
    pAnimation->gotoFrameAndPlay(0, true);
    
    lb_title->setString(g_strConnecting[g_nLangType]);
    pCurLayer->setVisible(true);
}

void ConnectingLayer::hideLayer()
{
    pCurLayer->stopAllActions();
    pCurLayer->setVisible(false);
}