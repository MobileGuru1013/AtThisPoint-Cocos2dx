//
//  GoogleService.hpp
//  Point_Online
//
//  Created by TianXinxing on 3/5/16.
//
//

#ifndef GoogleService_h
#define GoogleService_h

#include "common.h"

class Google {
    
public:
    
    static void startAndroidBonusNotification();
    static void cancelAndroidBonusNotification();
    static void startAndroidFreeSpinNotification();
    
    static void showProgressbar();
    static void hideProgressbar();
    
    static std::string getAndroidDeviceID();
};


#endif /* GoogleService_h */
