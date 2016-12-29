
#ifndef __PointGame_MyIAPListner__
#define __PointGame_MyIAPListner__

#include "common.h"
#include "PluginIAP/PluginIAP.h"

class MyIAPListner : public sdkbox::IAPListener, public Node
{
public:
    
    MyIAPListner(){}
    ~MyIAPListner(){}

    static MyIAPListner* getInstance();
    
    virtual void onSuccess(sdkbox::Product const& p) override;
    virtual void onFailure(sdkbox::Product const& p, const std::string &msg) override;
    virtual void onCanceled(sdkbox::Product const& p) override;
    virtual void onRestored(sdkbox::Product const& p) override;
    virtual void onProductRequestSuccess(std::vector<sdkbox::Product> const &products) override;
    virtual void onProductRequestFailure(const std::string &msg) override;
    virtual void onRestoreComplete(bool ok, const std::string &msg) override;
    
    virtual void onInitialized(bool ok) override;
    
    static MyIAPListner* _instance;
};

#endif /* defined(__PointGame_MyIAPListner__) */
