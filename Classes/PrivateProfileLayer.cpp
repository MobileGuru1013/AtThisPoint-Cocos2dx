//
//  PrivateProfileLayer.cpp
//  PointGame
//
//  Created by TianXinxing on 4/27/16.
//
//

#include "PrivateProfileLayer.h"

#include "WebService.h"
#include "Facebook.h"
#include "AppService.h"
#include "cJSON.h"

#include "HelloWorldScene.h"

//define string
std::string g_strPrivateProfile[6][2] = {{"PROFILE", "PERFIL"},
    {"Leaderboard Rank", "Ranking de Lideres"},
    {"Max Earning", "Ganancia Máxima"},
    {"Tournament Played", "Torneos Jugados"},
    {"Win Ratio", "Porcentaje de Victorias"},
    {"ACHIVEMENTS", "LOGROS"}
};

Layer* PrivateProfileLayer::createLayer(Layer *parent, int zorder, LayerDelegate *delegate)
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

bool PrivateProfileLayer::init()
{    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }
    
    auto rootNode = CSLoader::createNode("PrivateProfileLayer.csb");
    //rootNode->setScale(getScaleValX(), getScaleValY());
    pCurLayer->addChild(rootNode);
    
    lb_title[0] = (Text*)rootNode->getChildByName("title");
    
    lv_archives = (ListView*)rootNode->getChildByName("lv_list");
    lv_archives->setClippingEnabled(true);
    lv_archives->setClippingType(Layout::ClippingType::SCISSOR);
    
    auto panel_profile = (Layout*)lv_archives->getChildByName("panel_0");
    for (int i = 0; i < 4; i++) {
        
        lb_title[i+1] = (Text*)panel_profile->getChildByName(StringUtils::format("comment_%d", i));
    }
    auto panel_1 = (Layout*)lv_archives->getChildByName("panel_1");
    lb_title[5] = (Text*)panel_1->getChildByName("text");
    
    auto img_mark = (ImageView*)panel_profile->getChildByName("mark");
    img_mark->setScaleX(g_fScaleWH);

    auto imgPhotoPanel = (Layout*)(panel_profile->getChildByName("panel"));
    imgPhotoPanel->addTouchEventListener(CC_CALLBACK_2(PrivateProfileLayer::onMainBtnClicked, this, 2));
    imgPhotoPanel->setScaleX(g_fScaleWH);
    imgPhotoPanel->setLocalZOrder(10);
    
    auto image = (ImageView*)(imgPhotoPanel->getChildByName("photo"));
    image->setVisible(false);
    
    //clipping node
    auto clippingNode = ClippingNode::create();
    clippingNode->setAlphaThreshold(MASKING_THRESHOLD);
    clippingNode->setPosition(0, 0);
    clippingNode->setAnchorPoint(Vec2(0.0, 0.0));
    panel_profile->addChild(clippingNode);
    clippingNode->setLocalZOrder(5);
    
    auto mask = Sprite::createWithSpriteFrameName("img_mask.png");
    float scaleX = (imgPhotoPanel->getBoundingBox().size.width) / (float)mask->getBoundingBox().size.width;
    float scaleY = (imgPhotoPanel->getBoundingBox().size.height) / (float)mask->getBoundingBox().size.height;
    mask->setScale(scaleX, scaleY);
    mask->setPosition(imgPhotoPanel->getPosition());
    clippingNode->setStencil(mask);
    
    img_photo = ImageView::create(StringUtils::format("avatar_%d.jpg", 0), TextureResType::PLIST);
    float scaleX1 = (imgPhotoPanel->getBoundingBox().size.width) / (float)img_photo->getBoundingBox().size.width;
    float scaleY1 = (imgPhotoPanel->getBoundingBox().size.height) / (float)img_photo->getBoundingBox().size.height;
    img_photo->setScale(scaleX1, scaleY1);
    img_photo->setPosition(imgPhotoPanel->getPosition());
    clippingNode->addChild(img_photo);
    
    auto imgBorder = ImageView::create("border.png", TextureResType::PLIST);
    imgBorder->setScale(scaleX, scaleY);
    imgBorder->setPosition(imgPhotoPanel->getPosition());
    panel_profile->addChild(imgBorder);
    imgBorder->setLocalZOrder(12);
    
    //main buttons
    std::string strName = StringUtils::format("btn_%d", 0);
    auto btnMain = static_cast<Button*>(rootNode->getChildByName(strName));
    btnMain->addTouchEventListener(CC_CALLBACK_2(PrivateProfileLayer::onMainBtnClicked, this, 0));
    btnMain->setScaleX(g_fScaleWH);
    
    strName = StringUtils::format("btn_%d", 1);
    btnMain = static_cast<Button*>(panel_profile->getChildByName(strName));
    btnMain->addTouchEventListener(CC_CALLBACK_2(PrivateProfileLayer::onMainBtnClicked, this, 1));
    btnMain->setScaleX(g_fScaleWH);
    lb_fbtitle = (Text*)btnMain->getChildByName("text");
    
    lb_id = (Text*)(panel_profile->getChildByName("id"));
    lb_name = (Text*)(panel_profile->getChildByName("name"));
    lb_level = (Text*)(panel_profile->getChildByName("level"));
    lb_levelpoint = (Text*)(panel_profile->getChildByName("level_point"));
    lb_point = (Text*)(panel_profile->getChildByName("lb_point"));
    
    lb_rank = (Text*)(panel_profile->getChildByName("rank"));
    lb_rankpoint = (Text*)(panel_profile->getChildByName("lb_rankpoint"));
    img_mark = (ImageView*)lb_rankpoint->getChildByName("mark");
    img_mark->setScaleX(g_fScaleWH);
    lb_maxearning = (Text*)(panel_profile->getChildByName("lb_maxearning"));
    lb_tourplay = (Text*)(panel_profile->getChildByName("lb_tourplay"));
    lb_winpro = (Text*)(panel_profile->getChildByName("lb_winratio"));

    
    auto img_levelbar = (ImageView*)(panel_profile->getChildByName("levelbar"));
    level_bar = (LoadingBar*)(img_levelbar->getChildByName("pro_bar"));
    
    //init archivement list
    for (int i = 0; i < MAX_ARCHIVEMENTS; i++) {
        
        auto inputNode = CSLoader::createNode("ProfileCellNode.csb");
        auto panel = static_cast<Layout*>(inputNode->getChildByName("panel"));
        lb_achivTitle[i]= static_cast<Text*>(panel->getChildByName("title"));
        lb_achivComment[i] = (Text*)panel->getChildByName("comment");
        
        img_achivemark[i] = static_cast<ImageView*>(panel->getChildByName("mark"));
        img_achivemark[i]->setScaleX(g_fScaleWH);
        
        lb_archivLevel[i] = static_cast<Text*>(panel->getChildByName("level"));
        archiv_levelbar[i] = (LoadingBar*)panel->getChildByName("pro_bar");
        
        inputNode->removeChild(panel, true);
        lv_archives->pushBackCustomItem(panel);
    }
    
    //init avatar layer
    pAvatarLayer = new AvatarLayer();
    pAvatarLayer->createLayer(pCurLayer, 50, this);
    
    //init avatar layer
    pFBConnectLayer = new FBConnectLayer();
    pFBConnectLayer->createLayer(pCurLayer, 50, this);
    
    updateString2Lang();
    
    return true;
}

void PrivateProfileLayer::updateString2Lang()
{
    for (int i = 0; i < 6; i++) {
        
        lb_title[i]->setString(g_strPrivateProfile[i][g_nLangType]);
    }
}

void PrivateProfileLayer::onMainBtnClicked(Ref* pSender, Widget::TouchEventType type, int btn_id)
{
    if (type == Widget::TouchEventType::ENDED) {
        
        if (g_bSound == true) {
            
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("btn_click.aac", false);
        }
        if(btn_id == 0) { //close
            
            hideLayer();
        }
        else if(btn_id == 1) { //fb login
            
            if(g_bLoginFB == true) {
                
                AppService::showCircleProgressbar();
                
                hideLayer();                
                WebService::getInstance()->login2Guest();
            }
            else {
            
                pFBConnectLayer->showLayer();
            }
        }
        else if(btn_id == 2) { //avatar
            
            //hideLayer();
            pAvatarLayer->showLayer();
        }
    }
}

void PrivateProfileLayer::onUpdatedDone()
{
    if (child_index == AVATAR_LAYER) {
        
        //showLayer();
        updatInfo();
        g_pHelloWorld->showProfileInfo();
    }
    else if(child_index == FBCONNECT_LAYER) {
        
        hideLayer();
    }
}

void PrivateProfileLayer::getMyRank()
{
    std::string strURL = StringUtils::format("http://%s:8080/sys/money/mobile/getUserRank.do", WEBSERVICE_HOST);
    
    HttpRequest* request = new HttpRequest();
    request->setRequestType(HttpRequest::Type::POST);
    request->setUrl(strURL.c_str());
    request->setResponseCallback(CC_CALLBACK_2(PrivateProfileLayer::onGetMyRankDone, this));
    
    // write the post data
    std::string strData = StringUtils::format("USER_ID=%s",
                                              g_mainUserInfo.user_id.c_str()
                                              );
    request->setRequestData(strData.c_str(), strData.length());
    
    request->setTag("get Rank");
    //HttpClient::getInstance()->send(request);
    HttpClient::getInstance()->sendImmediate(request);
    
    request->release();
}

void PrivateProfileLayer::onGetMyRankDone(HttpClient* sender, HttpResponse* response)
{
    if (response->isSucceed() == false){
        
        return;
    }
    
    std::vector<char>buffer = *response->getResponseData();
    std::string str_msg = &buffer[0];
    
    //parsing data
    cJSON *json = cJSON_Parse(str_msg.c_str());
    if (json == NULL) {
        
        return;
    }
    
    cJSON *jsonRes = cJSON_GetObjectItem(json, "retcode");
    if (jsonRes->valueint != 0) {
        
        return;
    }
    
    g_mainUserInfo.rank = cJSON_GetObjectItem(json, "rank")->valueint;
    g_mainUserInfo.rank_point = cJSON_GetObjectItem(json, "point")->valuedouble;
    g_mainUserInfo.total_toprank = cJSON_GetObjectItem(json, "total_toprank")->valueint;
    
    cJSON_Delete(json);
    
//    if(g_mainUserInfo.rank == 0) {
//        
//        lb_rank->setString("--");
//        lb_rankpoint->setString("0");
//    }
//    else {
//        
//        lb_rank->setString(StringUtils::format("#%d", g_mainUserInfo.rank));
//        lb_rankpoint->setString(getNormalText2Number(g_mainUserInfo.rank_point));
//    }
    
    updatInfo();
}

void PrivateProfileLayer::updatInfo()
{
    //fb button
    if (g_bLoginFB == true) {
        
        lb_fbtitle->setString("LOG OUT");
    }
    else {
        
        lb_fbtitle->setString("LOG IN");
    }
    
    //update info
    WebService::getInstance()->setProfileImage(g_mainUserInfo.photo_id, img_photo);
    
    lb_id->setString(StringUtils::format("ID %s", g_mainUserInfo.user_id.c_str()));
    lb_name->setString(g_mainUserInfo.user_name);
    lb_level->setString(StringUtils::format("Level %d", g_mainUserInfo.level+1));
    lb_levelpoint->setString(getNormalText2Number(g_mainUserInfo.level_point));
    lb_point->setString(getNormalText2Number(g_mainUserInfo.points));
    
    if(g_mainUserInfo.rank == 0) {
        
        lb_rank->setString("--");
        lb_rankpoint->setString("0");
    }
    else {
        
        lb_rank->setString(StringUtils::format("#%d", g_mainUserInfo.rank));
        lb_rankpoint->setString(getNormalText2Number(g_mainUserInfo.rank_point));
    }
    
    float percent = g_mainUserInfo.level_point/(float)g_nLevelupXP[g_mainUserInfo.level]*100;
    level_bar->setPercent(percent);
    
    lb_maxearning->setString(getNormalText2Number(g_mainUserInfo.max_earning));
    lb_tourplay->setString(getNormalText2Number(g_mainUserInfo.total_tourplay));
    
    percent = 0;
    if(g_mainUserInfo.total_play > 0) {
        
        percent = g_mainUserInfo.win_play/(float)g_mainUserInfo.total_play*100;
    }
    lb_winpro->setString(StringUtils::format("%.0f %%", percent));
    
    //update archivements
    
    //1. total playing hour
    
    int index = 0;
    int level = 0;
    float cur_value, tar_value;
    
    if (g_nLangType == 0) {
    
        lb_achivTitle[index]->setString("Showtime");
        lb_achivComment[index]->setString("Hours Playing");
    }
    else {
        
        lb_achivTitle[index]->setString("Hora del show");
        lb_achivComment[index]->setString("Horas jugadas");
    }
    
    tar_value = 50;
    cur_value = g_mainUserInfo.total_hour/3600.0f;
    if(cur_value >= 150) {
        
        level = 3;
        cur_value = 150;
        tar_value = 150;
        
        if (g_nLangType == 0) {
         
            lb_achivTitle[index]->setString("Viral frame");
        } else {
            lb_achivTitle[index]->setString("Fama viral");
        }
    }
    else if(cur_value >= 100) {
        
        level = 2;
        tar_value = 150;
        
        if (g_nLangType == 0) {
         
            lb_achivTitle[index]->setString("Primetime");
        } else {
            lb_achivTitle[index]->setString("Hora estelar");
        }
    }
    else if(cur_value >= 50) {
        
        level = 1;
        tar_value = 100;
    }
    
    percent = cur_value/tar_value*100;
    if(level == 0) {
        
        //img_achivemark[index]->loadTexture("achiv_mark1.png", TextureResType::PLIST);
        img_achivemark[index]->setOpacity(120);
    }
    else {
        
        img_achivemark[index]->loadTexture(StringUtils::format("achiv_mark%d.png", level), TextureResType::PLIST);
    }
    archiv_levelbar[index]->setPercent(percent);
    lb_archivLevel[index]->setString(StringUtils::format("%s / %s", getNormalText2Number(cur_value).c_str(), getNormalText2Number(tar_value).c_str()));
    index++;
    
    //2. clean pot
    if (g_nLangType == 0) {
        
        lb_achivTitle[index]->setString("Pay day");
        lb_achivComment[index]->setString("Clean Pot");
    } else {
        lb_achivTitle[index]->setString("Cobrador");
        lb_achivComment[index]->setString("Potes limpiados");
    }
    level = 0;
    tar_value = 25;
    cur_value = g_mainUserInfo.total_cleanpot;
    if(g_mainUserInfo.total_cleanpot >= 150) {
        
        level = 3;
        cur_value = 150;
        tar_value = 150;
        if (g_nLangType == 0) {
         
            lb_achivTitle[index]->setString("Vacuum Cleaner");
        } else {
            lb_achivTitle[index]->setString("Aspiradora");
        }
    }
    else if(g_mainUserInfo.total_cleanpot >= 75) {
        
        level = 2;
        tar_value = 150;
        if (g_nLangType == 0) {
         
            lb_achivTitle[index]->setString("Point magnet");
        } else {
            lb_achivTitle[index]->setString("Atrae puntos");
        }
    }
    else if(g_mainUserInfo.total_cleanpot >= 25) {
        
        level = 1;
        tar_value = 75;
    }
    percent = cur_value/tar_value*100;
    if(level == 0) {
        
        //img_achivemark[index]->loadTexture("achiv_mark1.png", TextureResType::PLIST);
        img_achivemark[index]->setOpacity(120);
    }
    else {
        
        img_achivemark[index]->loadTexture(StringUtils::format("achiv_mark%d.png", level), TextureResType::PLIST);
    }
    archiv_levelbar[index]->setPercent(percent);
    lb_archivLevel[index]->setString(StringUtils::format("%s / %s", getNormalText2Number(cur_value).c_str(), getNormalText2Number(tar_value).c_str()));
    index++;
    
    //3. winning streak
    if (g_nLangType == 0) {
    
        lb_achivTitle[index]->setString("Lucky Shot");
        lb_achivComment[index]->setString("Winning Streak");
    } else {
        lb_achivTitle[index]->setString("Suertudo");
        lb_achivComment[index]->setString("Ganadas consecutivas");
    }
    level = 0;
    tar_value = 5;
    cur_value = g_mainUserInfo.win_streak;
    if(g_mainUserInfo.win_streak >= 10) {
        
        level = 3;
        cur_value = 10;
        tar_value = 10;
        if (g_nLangType == 0) {
         
            lb_achivTitle[index]->setString("Sophisticated Leprechaun hacker");
        } else {
            lb_achivTitle[index]->setString("Duende hacker");
        }
    }
    else if(g_mainUserInfo.win_streak >= 7) {
        
        level = 2;
        tar_value = 10;
        if (g_nLangType == 0) {
         
            lb_achivTitle[index]->setString("Four Leaf Clover Hat");
        } else {
            lb_achivTitle[index]->setString("Trébol de cuatro hojas");
        }
    }
    else if(g_mainUserInfo.win_streak >= 5) {
        
        level = 1;
        tar_value = 7;
    }
    percent = cur_value/tar_value*100;
    if(level == 0) {
        
        //img_achivemark[index]->loadTexture("achiv_mark1.png", TextureResType::PLIST);
        img_achivemark[index]->setOpacity(120);
    }
    else {
        
        img_achivemark[index]->loadTexture(StringUtils::format("achiv_mark%d.png", level), TextureResType::PLIST);
    }
    archiv_levelbar[index]->setPercent(percent);
    lb_archivLevel[index]->setString(StringUtils::format("%s / %s", getNormalText2Number(cur_value).c_str(), getNormalText2Number(tar_value).c_str()));
    index++;
    
    //4. total earning
    if (g_nLangType == 0) {
    
        lb_achivTitle[index]->setString("The rich duck uncle");
        lb_achivComment[index]->setString("Total Earnings");
    } else {
        lb_achivTitle[index]->setString("El tio rico pato");
        lb_achivComment[index]->setString("Puntos acumulados");
    }
    level = 0;
    tar_value = 100000;
    cur_value = g_mainUserInfo.total_earning;
    if(g_mainUserInfo.total_earning >= 1000000) {
        
        level = 3;
        cur_value = 1000000;
        tar_value = 1000000;
        if (g_nLangType == 0) {
         
            lb_achivTitle[index]->setString("Computer Rich Boy");
        } else {
            lb_achivTitle[index]->setString("Escobar");
        }
    }
    else if(g_mainUserInfo.total_earning >= 500000) {
        
        level = 2;
        tar_value = 1000000;
        if (g_nLangType == 0) {
            
            lb_achivTitle[index]->setString("Ricky");
        } else {
            lb_achivTitle[index]->setString("Ricky");
        }
    }
    else if(g_mainUserInfo.total_earning >= 100000) {
        
        level = 1;
        tar_value = 500000;
    }
    percent = cur_value/tar_value*100;
    if(level == 0) {
        
        //img_achivemark[index]->loadTexture("achiv_mark1.png", TextureResType::PLIST);
        img_achivemark[index]->setOpacity(120);
    }
    else {
        
        img_achivemark[index]->loadTexture(StringUtils::format("achiv_mark%d.png", level), TextureResType::PLIST);
    }
    archiv_levelbar[index]->setPercent(percent);
    lb_archivLevel[index]->setString(StringUtils::format("%s / %s", getNormalText2Number(cur_value).c_str(), getNormalText2Number(tar_value).c_str()));
    index++;
    
    //5. tournament played
    if (g_nLangType == 0) {
    
        lb_achivTitle[index]->setString("Heroe");
        lb_achivComment[index]->setString("Tournament Played");
    } else {
        lb_achivTitle[index]->setString("Héroe");
        lb_achivComment[index]->setString("Torneos jugados");
    }
    level = 0;
    tar_value = 5;
    cur_value = g_mainUserInfo.total_tourplay;
    if(g_mainUserInfo.total_tourplay >= 25) {
        
        level = 3;
        cur_value = 25;
        tar_value = 25;
        if (g_nLangType == 0) {
         
            lb_achivTitle[index]->setString("Legend");
        } else {
            lb_achivTitle[index]->setString("Leyenda");
        }
    }
    else if(g_mainUserInfo.total_tourplay >= 15) {
        
        level = 2;
        tar_value = 25;
        if (g_nLangType == 0) {
         
            lb_achivTitle[index]->setString("Glory");
        } else {
            lb_achivTitle[index]->setString("Vanagloriado");
        }
    }
    else if(g_mainUserInfo.total_tourplay >= 5) {
        
        level = 1;
        tar_value = 15;
    }
    percent = cur_value/tar_value*100;
    if(level == 0) {
        
        //img_achivemark[index]->loadTexture("achiv_mark1.png", TextureResType::PLIST);
        img_achivemark[index]->setOpacity(120);
    }
    else {
        
        img_achivemark[index]->loadTexture(StringUtils::format("achiv_mark%d.png", level), TextureResType::PLIST);
    }
    archiv_levelbar[index]->setPercent(percent);
    lb_archivLevel[index]->setString(StringUtils::format("%s / %s", getNormalText2Number(cur_value).c_str(), getNormalText2Number(tar_value).c_str()));
    index++;
    
    //6. win play
    if (g_nLangType == 0) {
    
        lb_achivTitle[index]->setString("Manager");
        lb_achivComment[index]->setString("Wins Amount");
    } else {
        lb_achivTitle[index]->setString("Gerente");
        lb_achivComment[index]->setString("Victorias");
    }
    level = 0;
    tar_value = 500;
    cur_value = g_mainUserInfo.win_play;
    if(g_mainUserInfo.win_play >= 2000) {
        
        level = 3;
        cur_value = 2000;
        tar_value = 2000;
        if (g_nLangType == 0) {
         
            lb_achivTitle[index]->setString("CEO");
        } else {
            lb_achivTitle[index]->setString("El Amo");
        }
    }
    else if(g_mainUserInfo.win_play >= 1000) {
        
        level = 2;
        tar_value = 2000;
        if (g_nLangType == 0) {
         
            lb_achivTitle[index]->setString("President");
        } else {
            lb_achivTitle[index]->setString("Presidente");
        }
    }
    else if(g_mainUserInfo.win_play >= 500) {
        
        level = 1;
        tar_value = 1000;
    }
    percent = cur_value/tar_value*100;
    if(level == 0) {
        
        //img_achivemark[index]->loadTexture("achiv_mark1.png", TextureResType::PLIST);
        img_achivemark[index]->setOpacity(120);
    }
    else {
        
        img_achivemark[index]->loadTexture(StringUtils::format("achiv_mark%d.png", level), TextureResType::PLIST);
    }
    archiv_levelbar[index]->setPercent(percent);
    lb_archivLevel[index]->setString(StringUtils::format("%s / %s", getNormalText2Number(cur_value).c_str(), getNormalText2Number(tar_value).c_str()));
    index++;
    
    //7. emoticon sent
    if (g_nLangType == 0) {
    
        lb_achivTitle[index]->setString("Radio host");
        lb_achivComment[index]->setString("Emoticons Sent");
    } else {
        lb_achivTitle[index]->setString("Anime");
        lb_achivComment[index]->setString("Expresiones enviadas");
    }
    level = 0;
    tar_value = 50;
    cur_value = g_mainUserInfo.total_emoticon;
    if(g_mainUserInfo.total_emoticon >= 500) {
        
        level = 3;
        cur_value = 500;
        tar_value = 500;
        if (g_nLangType == 0) {
         
            lb_achivTitle[index]->setString("Movie Star");
        } else {
            lb_achivTitle[index]->setString("Diva");
        }
    }
    else if(g_mainUserInfo.total_emoticon >= 250) {
        
        level = 2;
        tar_value = 500;
        if (g_nLangType == 0) {
         
            lb_achivTitle[index]->setString("TV Host");
        } else {
            lb_achivTitle[index]->setString("Telenovela");
        }
    }
    else if(g_mainUserInfo.total_emoticon >= 50) {
        
        level = 1;
        tar_value = 250;
    }
    percent = cur_value/tar_value*100;
    if(level == 0) {
        
        //img_achivemark[index]->loadTexture("achiv_mark1.png", TextureResType::PLIST);
        img_achivemark[index]->setOpacity(120);
    }
    else {
        
        img_achivemark[index]->loadTexture(StringUtils::format("achiv_mark%d.png", level), TextureResType::PLIST);
    }
    archiv_levelbar[index]->setPercent(percent);
    lb_archivLevel[index]->setString(StringUtils::format("%s / %s", getNormalText2Number(cur_value).c_str(), getNormalText2Number(tar_value).c_str()));
    index++;
    
    //8. avatars
    if (g_nLangType == 0) {
        
        lb_achivTitle[index]->setString("Zookeaper");
        lb_achivComment[index]->setString("Amount of Avatars");
    } else {
        lb_achivTitle[index]->setString("Zoólogo");
        lb_achivComment[index]->setString("Avatares Comprados");
    }
    
    int cntAvatar = 0;
    for (int i = 0; i < MAX_AVATARS; i++) {
        
        int value = getAvatarUnlock(i);
        if (value == 0) continue;
        
        cntAvatar++;
    }
    
    level = 0;
    tar_value = 3;
    cur_value = cntAvatar;
    
    if(cntAvatar >= 12) {
        
        level = 3;
        cur_value = 12;
        tar_value = 12;
        if (g_nLangType == 0) {
         
            lb_achivTitle[index]->setString("Catch Them All");
        } else {
            lb_achivTitle[index]->setString("Atraparlos ya");
        }
    }
    else if(cntAvatar >= 8) {
        
        level = 2;
        tar_value = 12;
        if (g_nLangType == 0) {
         
            lb_achivTitle[index]->setString("Tarzan");
        } else {
            lb_achivTitle[index]->setString("Tarzan");
        }
    }
    else if(cntAvatar >= 3) {
        
        level = 1;
        tar_value = 8;
    }
    percent = cur_value/tar_value*100;
    if(level == 0) {
        
        //img_achivemark[index]->loadTexture("achiv_mark1.png", TextureResType::PLIST);
        img_achivemark[index]->setOpacity(120);
    }
    else {
        
        img_achivemark[index]->loadTexture(StringUtils::format("achiv_mark%d.png", level), TextureResType::PLIST);
    }
    archiv_levelbar[index]->setPercent(percent);
    lb_archivLevel[index]->setString(StringUtils::format("%s / %s", getNormalText2Number(cur_value).c_str(), getNormalText2Number(tar_value).c_str()));
    index++;
    
    //9. total toprank
    if (g_nLangType == 0) {
    
        lb_achivTitle[index]->setString("Incredible News");
        lb_achivComment[index]->setString("Appears on Leaderboard");
    } else {
        lb_achivTitle[index]->setString("Increible noticia");
        lb_achivComment[index]->setString("Llegar al Ranking");
    }
    level = 0;
    tar_value = 2;
    cur_value = g_mainUserInfo.total_toprank;
    if(g_mainUserInfo.total_toprank >= 10) {
        
        level = 3;
        cur_value = 10;
        tar_value = 10;
        if (g_nLangType == 0) {
         
            lb_achivTitle[index]->setString("Staring from the Bottom");
        } else {
            lb_achivTitle[index]->setString("Nadie puede creerlo");
        }
    }
    else if(g_mainUserInfo.total_toprank >= 5) {
        
        level = 2;
        tar_value = 8;
        if (g_nLangType == 0) {
         
            lb_achivTitle[index]->setString("Really?");
        } else {
            lb_achivTitle[index]->setString("¿En serio?");
        }
    }
    else if(g_mainUserInfo.total_toprank >= 2) {
        
        level = 1;
        tar_value = 5;
    }
    percent = cur_value/tar_value*100;
    if(level == 0) {
        
        //img_achivemark[index]->loadTexture("achiv_mark1.png", TextureResType::PLIST);
        img_achivemark[index]->setOpacity(120);
    }
    else {
        
        img_achivemark[index]->loadTexture(StringUtils::format("achiv_mark%d.png", level), TextureResType::PLIST);
    }
    archiv_levelbar[index]->setPercent(percent);
    lb_archivLevel[index]->setString(StringUtils::format("%s / %s", getNormalText2Number(cur_value).c_str(), getNormalText2Number(tar_value).c_str()));
    index++;
    
    //9. friend invite
    if (g_nLangType == 0) {
    
        lb_achivTitle[index]->setString("Pizza Party");
        lb_achivComment[index]->setString("Friends Invites");
    } else {
        lb_achivTitle[index]->setString("Fiesta de pizza");
        lb_achivComment[index]->setString("Amigos invitados");
    }
    level = 0;
    tar_value = 50;
    cur_value = g_mainUserInfo.total_invite;
    if(g_mainUserInfo.total_invite >= 250) {
        
        level = 3;
        cur_value = 250;
        tar_value = 250;
        if (g_nLangType == 0) {
         
            lb_achivTitle[index]->setString("Sweet 16");
        } else {
            lb_achivTitle[index]->setString("Quiero mis 15");
        }
    }
    else if(g_mainUserInfo.total_invite >= 100) {
        
        level = 2;
        tar_value = 250;
        if (g_nLangType == 0) {
         
            lb_achivTitle[index]->setString("Friday Night");
        } else {
            lb_achivTitle[index]->setString("Viernes en la noche");
        }
    }
    else if(g_mainUserInfo.total_invite >= 50) {
        
        level = 1;
        tar_value = 100;
    }
    percent = cur_value/tar_value*100;
    img_achivemark[index]->setOpacity(105+level*50);
    archiv_levelbar[index]->setPercent(percent);
    lb_archivLevel[index]->setString(StringUtils::format("%s / %s", getNormalText2Number(cur_value).c_str(), getNormalText2Number(tar_value).c_str()));
    index++;
}

void PrivateProfileLayer::showLayer()
{
    getMyRank();
    updatInfo();
    
    updateString2Lang();
    pCurLayer->setVisible(true);
}

void PrivateProfileLayer::hideLayer()
{
     pCurLayer->setVisible(false);
}
