//
//  AppService.hpp
//  PointGame
//
//  Created by TianXinxing on 4/20/16.
//
//

#ifndef AppService_hpp
#define AppService_hpp

#include "common.h"

class AppService {
    
public:
    
    static void showCircleProgressbar();
    static void hideCircleProgressbar();
    
    static std::string getDeviceID();
    
    static void startFreeSpinNotification();
    static void startBonusNotification();
    static void cancelBonusNotification();
};

#endif /* AppService_hpp */
