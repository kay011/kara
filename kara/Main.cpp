/**
 * @Author Karate Yuan
 * @Email haodong_yuan@163.com
 * @Date: 2020/6/12
 */

#include <getopt.h>
#include <string>
#include "EventLoop.h"
#include "Server.h"
#include "base/Logging.h"

int main(int argc, char *argv[]) {
  int threadNum = 2;
  int port = 8088;
  std::string logPath = "./KaraServer.log";
  int opt;
  const char *str = "t:l:p:";
  while ((opt = getopt(argc, argv, str)) != -1) {
    switch (opt) {
      case 't': {
        threadNum = atoi(optarg);
        break;
      }
      case 'l': {
        logPath = optarg;
        if (logPath.size() < 2 || optarg[0] != '/') {
          printf("logPath should start with \"/\"\n");
          abort();
        }
        break;
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
  // mianloop没有放到线程池中
  EventLoop mainLoop;
  Server myHTTPServer(&mainLoop, threadNum, port);
  myHTTPServer.start();
  mainLoop.loop();
  return 0;
}
