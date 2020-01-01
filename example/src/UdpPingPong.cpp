#include <thread>
#include <mutex>
#include <vector>
#include <string>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "UdpSocket.h"


static void PingThread (UdpSocket &newSocket);
static void PongThread (UdpSocket &newSocket);

//*****************************************************
// main
//*****************************************************
int main() {
  std::string ipAddr("localhost");
  std::string port("8889");
  UdpSocket pingSocket (ipAddr, port);
  UdpSocket pongSocket (ipAddr, port);

  pingSocket.EnableDebug(true);
  pongSocket.EnableDebug(true);
  
  std::thread ping (PingThread, std::ref(pingSocket));
  std::thread pong (PongThread, std::ref(pongSocket));

  ping.join();
  pong.join();
  
  printf ("Exiting\n");
  return 0;
}


//*****************************************************
// PingThread
//*****************************************************
void PingThread (UdpSocket &udpSocket) {
  uint8_t buffer[200];
  int counter{0};
  printf ("Ping Thread started\n");
  int retries{3};
  bool running {true};

  while (running) {
    std::string msg = std::string ("PING ") + std::to_string(counter++);
    printf ("Sending '%s'\n", msg.c_str());
    while (udpSocket.SendTo(msg.c_str(), msg.length()) < 0 && retries) {
      --retries;
      sleep(1); // start us off trying to send
      if (retries == 0) {
        printf ("Unable to Initiate Ping Pong Test\n");
        return ;
      }
    }
    sleep(1); // start us off trying to send
    retries = 0;// disable after test started.
#if 0
    int rval = udpSocket.RecvFrom(buffer, sizeof(buffer));
    if (rval > 0) {
       buffer[rval] = 0;
       printf("PING received: '%s'\n", buffer);
     }
#endif
  }
  std::string msg = "stop";
  udpSocket.SendTo (msg.c_str(), msg.size());
  udpSocket.Shutdown(SHUT_RDWR);
  printf ("Ping Thread Exiting\n");
}


//*****************************************************
// PongThread
//*****************************************************
void PongThread (UdpSocket &udpSocket) {
  uint8_t buffer[200];

  printf ("Pong Thread started\n");

  bool running {true};
  while (running) {
    printf ("Pong calling RecvFrom\n");
    int rval = udpSocket.RecvFrom(buffer, sizeof(buffer));
    if (rval > 0) {
       buffer[rval] = 0;
       printf("Pong: '%s'\n", buffer);
#if 0       
       if (udpSocket.SendTo(buffer, rval) <= 0) {
         printf ("Pong failed to send %s\n", strerror(errno));
         running = false;
       }
#endif       
     } else {
        printf ("Pong failed to receive %s\n", strerror(errno));
        running = false;
     }
     if (std::string((char*)buffer) == std::string("stop")) {
        printf ("Pong received stop signal\n");
        running = false;
     }
     
  }
  udpSocket.Shutdown(SHUT_RDWR);
  printf ("Pong Thread Exiting\n");
}