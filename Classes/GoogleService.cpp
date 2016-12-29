//
//  GoogleService.cpp
//  Point_Online
//
//  Created by TianXinxing on 3/5/16.
//
//

#include "GoogleService.h"

#include "HelloWorldScene.h"
#include "WebService.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <android/log.h>

#endif

const char* AppActivityClassName = "com/xinxing/point/AppActivity";

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

extern "C"
{
    JNIEXPORT void JNICALL  Java_com_xinxing_point_AppActivity_nativeGetDeviceIDCallback(JNIEnv*  env, jobject thiz, jstring dev_id)
    {
        const char* str_id = env->GetStringUTFChars(dev_id, NULL);
        
        CCLOG("Device ID: %s", str_id);
        
        //cocos2d::MessageBox(str_token, "Alert");
        
        g_strDeviceID = str_id;
        env->ReleaseStringUTFChars(dev_id, str_id);
        
        //CCLOG("Device Token: setstring", str_id);
        
        //saveSettingData();
        
        //CCLOG("Device Token: save", str_token);
        //WebService::getInstance()->updateToken();
    }

	JNIEXPORT void JNICALL  Java_com_xinxing_point_AppActivity_nativeGetTokenCallback(JNIEnv*  env, jobject thiz, jstring token)
    {
        const char* str_token = env->GetStringUTFChars(token, NULL);
        
        CCLOG("Device Token: %s", str_token);
        
        //cocos2d::MessageBox(str_token, "Alert");
        
        g_strDeviceToken = str_token;
        env->ReleaseStringUTFChars(token, str_token);
        
        CCLOG("Device Token: save %s", str_token);
        WebService::getInstance()->updateToken();
    }
    
	JNIEXPORT void JNICALL Java_com_xinxing_point_AppActivity_nativeReceiveFriendPushCallback(JNIEnv*  env, jobject thiz,
                                                                           jstring user_id, jstring user_name, jstring user_photo, jstring user_level, jstring user_point)
    {
        const char* str_id = env->GetStringUTFChars(user_id, NULL);
        const char* str_name = env->GetStringUTFChars(user_name, NULL);
        const char* str_photo = env->GetStringUTFChars(user_photo, NULL);
        const char* str_level = env->GetStringUTFChars(user_level, NULL);
        const char* str_point = env->GetStringUTFChars(user_point, NULL);
        
        //cocos2d::MessageBox("Friend Push", "Alert");
        
        FRIEND_INFO info;
        info.fb_id = str_id;
        info.user_name = str_name;
        info.photo_id = str_photo;
        info.level = atoi(str_level);
        info.points = atof(str_point);
        
        env->ReleaseStringUTFChars(user_id, str_id);
        env->ReleaseStringUTFChars(user_name, str_name);
        env->ReleaseStringUTFChars(user_photo, str_photo);
        env->ReleaseStringUTFChars(user_level, str_level);
        env->ReleaseStringUTFChars(user_point, str_point);
        
        addFriend(info);
        
        CCLOG("Friend Push Message");
        
        if(g_nLangType == 0) {
            
            g_pHelloWorld->actionNewFriend(StringUtils::format("You and %s \nbecome game buddy", info.user_name.c_str()));
        } else {
            
            g_pHelloWorld->actionNewFriend(StringUtils::format("Tu y %s \nse han hecho amigos", info.user_name.c_str()));
        }
    }
    
	JNIEXPORT void JNICALL Java_com_xinxing_point_AppActivity_nativeReceiveMsgPushCallback(JNIEnv*  env, jobject thiz,
                                                  jstring user_id, jstring user_name, jstring user_photo, jstring msg_index, jstring msg_type, jstring msg_title, jstring msg_content)
    {
        const char* str_id = env->GetStringUTFChars(user_id, NULL);
        const char* str_name = env->GetStringUTFChars(user_name, NULL);
        const char* str_photo = env->GetStringUTFChars(user_photo, NULL);
        const char* str_index = env->GetStringUTFChars(msg_index, NULL);
        const char* str_type = env->GetStringUTFChars(msg_type, NULL);
        const char* str_title = env->GetStringUTFChars(msg_title, NULL);
        const char* str_msg = env->GetStringUTFChars(msg_content, NULL);
        
        //cocos2d::MessageBox("Invite Push", "Alert");
        
        time_t cur_time;
        time(&cur_time);
        
        MSG_INFO info;
        info.index = atoi(str_index);
        info.sender_id = str_id;
        info.name = str_name;
        info.photo_id = str_photo;
        info.title = str_title;
        info.msg = str_msg;
        info.type = atoi(str_type);
        info.sent_time = cur_time;
        
        bool flag = addMsg(info);
        
        env->ReleaseStringUTFChars(user_id, str_id);
        env->ReleaseStringUTFChars(user_name, str_name);
        env->ReleaseStringUTFChars(user_photo, str_photo);
        env->ReleaseStringUTFChars(msg_index, str_index);
        env->ReleaseStringUTFChars(msg_type, str_type);
        env->ReleaseStringUTFChars(msg_title, str_title);
        env->ReleaseStringUTFChars(msg_content, str_msg);
        
        if (flag == false) return;
        
        if(g_pHelloWorld != NULL) {
            
            //g_pHelloWorld->actionNewMsg("New message is received");
            if(g_nLangType == 0) {
                
                if(info.type == 0) {
                    
                    g_pHelloWorld->actionNewMsg("You have a new message");
                } else if(info.type == 1) {
                    g_pHelloWorld->actionNewMsg(StringUtils::format("%s has sent you\n a friend request", info.name.c_str()));
                } else if(info.type == 2) {
                    g_pHelloWorld->actionNewMsg(StringUtils::format("%s invites you\n to join a table", info.name.c_str()));            }
            } else {
                
                if(info.type == 0) {
                    
                    g_pHelloWorld->actionNewMsg("Tienes un nuevo mensaje");
                } else if(info.type == 1) {
                    g_pHelloWorld->actionNewMsg(StringUtils::format("%s te ha enviado\n una solicitud de amistad", info.name.c_str()));
                } else if(info.type == 2) {
                    g_pHelloWorld->actionNewMsg(StringUtils::format("%s te ha invitado\n a una mesa", info.name.c_str()));
                }
            }
        }
        
        CCLOG("Msg Push");
    }
    
    JNIEXPORT void JNICALL Java_com_xinxing_point_AppActivity_nativeReceiveGiftPushCallback(JNIEnv*  env, jobject thiz,
                                                                                          jstring gift_index, jstring gift_type, jstring gift_point)
    {
        const char* str_index = env->GetStringUTFChars(gift_index, NULL);
        const char* str_type = env->GetStringUTFChars(gift_type, NULL);
        const char* str_bonus = env->GetStringUTFChars(gift_point, NULL);
        
        //cocos2d::MessageBox("Invite Push", "Alert");
        
//        time_t cur_time;
//        time(&cur_time);
        
        GIFT_INFO info;
        info.index = atoi(str_index);
        info.type = atoi(str_type);
        info.bonus = atoi(str_bonus);
        
        bool flag = addGift(info);
        
        env->ReleaseStringUTFChars(gift_index, str_index);
        env->ReleaseStringUTFChars(gift_type, str_type);
        env->ReleaseStringUTFChars(gift_point, str_bonus);
        
        if (flag == false) return;
        
        WebService::getInstance()->updateMsgGiftInfo();
        if(g_pHelloWorld != NULL) {
            
            //g_pHelloWorld->actionNewGift("New Gift is received");
            if(g_nLangType == 0) {
                
                g_pHelloWorld->actionNewGift("You have received a gift");
            } else {
                g_pHelloWorld->actionNewGift("Has recibido un regalo");
            }
        }
        
        CCLOG("Gift Push");
    }
        
    JNIEXPORT void JNICALL Java_com_xinxing_point_AppActivity_nativeReceiveAlarmPushCallback(JNIEnv*  env, jobject thiz,
                                                                                            jint alarm_index)
    {
        int index = alarm_index;
        if (index == 101) { //bonus alarm
            
            if (g_pHelloWorld != NULL) {
                
                if (g_nLangType == 0) {
                    
                    g_pHelloWorld->actionNewBonus("Claim now your free points\n and keep playing");
                } else {
                    g_pHelloWorld->actionNewBonus("Reclama ahora tus puntos\n gratis y sigue");
                }
            }
            
        } else if(index == 102) { //free spin alarm
            
            g_nFreeSpin++;
            time(&g_timeLastFreeSpin);
            WebService::getInstance()->updateFreeSpin();
            if (g_pHelloWorld != NULL) {
             
                if (g_nLangType == 0) {
                    
                    g_pHelloWorld->actionNewFreeSpin("You have a free spin.\nTry your luck now!");
                } else {
                    g_pHelloWorld->actionNewFreeSpin("Tienes un giro gratis.\nÚsalo Ahora!");
                }
            }
        }
        
        CCLOG("Alarm Push");
    }

};

#endif

void Google::startAndroidBonusNotification()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, AppActivityClassName, "nativeStartBonusLocalNotification", "()V")) {
        t.env->CallStaticVoidMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
    }
    
#endif
}

void Google::cancelAndroidBonusNotification()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, AppActivityClassName, "nativeCancelBonusLocalNotification", "()V")) {
        t.env->CallStaticVoidMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
    }
    
#endif
}

void Google::startAndroidFreeSpinNotification()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, AppActivityClassName, "nativeStartFreeSpinLocalNotification", "()V")) {
        t.env->CallStaticVoidMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
    }
    
#endif

}

void Google::showProgressbar()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, AppActivityClassName, "nativeShowProgressbar", "()V")) {
        t.env->CallStaticVoidMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
    }
    
#endif
}

void Google::hideProgressbar()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, AppActivityClassName, "nativeHideProgressbar", "()V")) {
        t.env->CallStaticVoidMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
    }
    
#endif
}

std::string Google::getAndroidDeviceID()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, AppActivityClassName, "nativeGetDeviceID", "()Ljava/lang/String;")) {
        
        jstring jstr = NULL;
        jstr = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
        char* cstr = NULL;
        cstr = (char*) t.env->GetStringUTFChars(jstr, 0);
        std::string ret(cstr);
        t.env->ReleaseStringUTFChars(jstr, cstr);
        t.env->DeleteLocalRef(jstr);
        
        return ret;
   }
    
#endif
}