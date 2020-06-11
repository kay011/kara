/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/6/7
 */
#include <getopt.h>
#include <string>
#include "EventLoop.h"
#include "Server.h"
#include "base/Logging.h"

int main(int argc, char *argv[])
{
    int threadNum = 4;
    int port = 80;
    std::string logPath = "./KaraServer.log";
    int opt;
    const char* str = "t:l:p:";
    while((opt = getopt(argc, argv, str)) != -1){
        switch (opt)
        {
        case 't': {
            threadNum = atoi(optarg);
            break;
        }
            
        case 'l': {
            logPath = optarg;
            if(logPath.size() < 2 ||  optarg[0] != '/'){
                printf("logpath should start with \"/\" \n");
                abort();
            }
        }
        case 'p': {
            port = atoi(optarg);
            break;
        }

        default:
            break;
        }
    }
    Logger::setLogFileName(logPath);
    EventLoop mainLoop;
    Server myHttpServer(&mainLoop, threadNum, port);
    myHttpServer.start();
    mainLoop.loop();
    return 0;
}
