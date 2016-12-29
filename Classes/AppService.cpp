//
//  AppService.cpp
//  PointGame
//
//  Created by TianXinxing on 4/20/16.
//
//

#include "AppService.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "AppleService.h"
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "GoogleService.h"
#endif

void AppService::showCircleProgressbar()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    showProgressbar();
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    Google::showProgressbar();
#endif
}

void AppService::hideCircleProgressbar()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    hideProgressbar();
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    Google::hideProgressbar();
#endif
}

std::string AppService::getDeviceID()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

    return getAppleDeviceID();
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
    return g_strDeviceID;
    //return Google::getAndroidDeviceID();
#endif
}

void AppService::startFreeSpinNotification()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    
    startAppleFreeSpinNotification();
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
    Google::startAndroidFreeSpinNotification();
#endif
}

void AppService::startBonusNotification()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    
    startAppleBonusNotification();
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
    Google::startAndroidBonusNotification();
#endif
}

void AppService::cancelBonusNotification()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    
    cancelAppleBonusNotification();
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
    Google::cancelAndroidBonusNotification();
#endif
}