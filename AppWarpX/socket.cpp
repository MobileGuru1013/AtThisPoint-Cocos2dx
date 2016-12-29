/*
 Copyright (c) 2013 Shephertz Technologies Pvt. Ltd.
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 The Software shall be used for Good, not Evil.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */
#include "socket.h"
#include <fcntl.h>
#include "appwarp.h"
#include <errno.h>

typedef struct{
    
    int len;
    char msg[512];
    
}SENDINGMSG;

static std::mutex       g_sendQueueMutex_1;
static std::mutex       g_responseQueueMutex_1;
static std::mutex       g_responseMsgMutex_1;

static std::mutex       g_SleepMutex_1;
static std::condition_variable      g_SleepCondition_1;

static bool g_need_quit_1 = false;

static std::queue<std::string> g_responseQueue_1;
static std::queue<SENDINGMSG> g_sendQueue_1;

static std::string g_responseMsg_1 = "";

namespace AppWarp
{
	namespace Utility
	{
        
		Socket::Socket(Client* owner)
		{
            _callBack = owner;
            sockd = -1;
		}

		Socket::~Socket()
		{
		}
      
		int Socket::sockConnect(std::string host, short port)
		{
		    sockd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockd == -1)
            {
                return AppWarp::result_failure;
            }
            
            serv_name.sin_family = AF_INET;
            
            inet_aton(host.c_str(), &serv_name.sin_addr);
            serv_name.sin_port = htons(port);

            int status = connect(sockd, (struct sockaddr*)&serv_name, sizeof(serv_name));
        
            if (status == -1)
            {
                return AppWarp::result_failure;
            }
            
            fcntl(sockd, F_SETFL, O_NONBLOCK);
            
            return AppWarp::result_success;
		}

		int Socket::sockDisconnect()
		{
            if(sockd == -1)
            {
                return AppWarp::result_failure;
            }
            shutdown(sockd, SHUT_RDWR);
            close(sockd);
            
            g_need_quit_1 = true;
            
            return AppWarp::result_success;
        }

		int Socket::sockSend(char *messageToSend,int messageLength)
		{
            g_sendQueueMutex_1.lock();
            
            AppWarp::Client::getInstance()->keep_time = clock();
            AppWarp::Client::getInstance()->keepAliveWatchDog = false;
            //_callBack->keep_time = clock();
            //_callBack->keepAliveWatchDog = false;
            
            int bytes_sent = send(sockd, messageToSend, messageLength, 0);
            
            g_sendQueueMutex_1.unlock();
            
            if(bytes_sent != messageLength)
            {
                CCLOG("Send Message Failed !!!");
                return AppWarp::result_failure;
            }
            else
            {
                CCLOG("Send Message Success !!!");
                return AppWarp::result_success;
            }
		}

		void Socket::checkMessages()
		{
            unsigned char msg[4096];
			int ret = recv(sockd, msg, 4096, 0);
            
            if(ret > 0)
            {
                //record last receive time
                //_callBack->keep_time = clock();
                //_callBack->socketNewMsgCallback(msg, ret);
                
                AppWarp::Client::getInstance()->lastReceiveTime = clock(); //record last receive message
                
                AppWarp::Client::getInstance()->socketNewMsgCallback(msg, ret);
            }
            else if(errno == EWOULDBLOCK)
            {
                return;
            }
            else
            {
                //g_sendSocketMutex.lock();
                
                //_callBack->socketConnectionCallback(AppWarp::result_failure);
                AppWarp::Client::getInstance()->socketConnectionCallback(AppWarp::result_failure);
                
                //g_sendSocketMutex.unlock();
            }
		}
        
	}
}