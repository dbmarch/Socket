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
#include "time.h"

static void UdpServerThread (UdpSocket &newSocket);



class PingPongTest {
public:
    PingPongTest() {};
    ~PingPongTest(){};
    

   void ReceiveThread(UdpSocket &udpSocket) {
      if (mDebug) printf ("PingPongTest Receive Thread started\n");
      char buffer[200];
      bool receiving{true};
      while (receiving) {
        int rval = udpSocket.RecvFrom(buffer, sizeof(buffer));
        if (rval > 0) {
          const std::lock_guard<std::mutex> lock(mMutex);

    			buffer[rval] = 0;
          std::string msg{buffer};
          printf ("Put %s in MQ\n", msg.c_str());
          mMQ.push_back(std::move(msg));
        } else {
            fprintf(stderr, "Receive Error: %s\n", strerror(errno));
            receiving = false;
         } 
      }
      printf ("Receive Thread Exiting\n");
   }

 void MainThread (UdpSocket &udpSocket) {
    if (mDebug) printf ("Udp Test Main Thread  started.\n");
    bool waitingReply{false};
    bool running{true};
    int counter{0};
    int timeoutCounter{0};
    while (running)
    {
      if (waitingReply) {
          const std::lock_guard<std::mutex> lock(mMutex);
          if (mMQ.size()) {
            for (std::string msg : mMQ) {
              printf ("UDP RX: '%s'\n", msg.c_str());
            }
            mMQ.clear();
          }
          // We should use std::chrono to timestamp.
          // wait 100msec for now...
          struct timespec tv = {.tv_sec = 0, .tv_nsec = 100000};
          struct timespec tv2;
          nanosleep(&tv, &tv2);
          timeoutCounter--;
          if (timeoutCounter == 0) {
            waitingReply = false;
            printf ("TIMEOUT\n");
          }
      } else {
      std::string msg = "UDP TX # " + std::to_string(counter);
      if (udpSocket.SendTo(msg.c_str(), msg.size())< 0) {
        fprintf (stderr,"Udp Failed to send packet %s\n", strerror(errno));
        running = false;
      } else {
        waitingReply = true;
        counter++;
        timeoutCounter = 100;
      }

      }

    }
  }

 void RunPingPongTest(UdpSocket &udpSocket) {
    std::thread main (&PingPongTest::MainThread, this,  std::ref(udpSocket));
    std::thread receive (&PingPongTest::ReceiveThread, this,  std::ref(udpSocket));
    main.join();
    receive.join();
    printf ("ProcessMessages Exiting\n");
 }

protected:
  std::mutex mMutex;
  std::vector<std::string> mMQ{};
  bool mDebug {false};
};



//*****************************************************
// main
//*****************************************************
int main() {
  std::string ipAddr("localhost");
  std::string port("8900");
  UdpSocket udpSocket;
  udpSocket.EnableDebug(false);
  udpSocket.Bind (ipAddr, port);
  
  std::thread udpServerThread (UdpServerThread, std::ref(udpSocket));
  
  PingPongTest pingpong;

  pingpong.RunPingPongTest(udpSocket);

  udpServerThread.join();
  
  printf ("Exiting\n");
  return 0;
}


//*****************************************************
// PingThread
//*****************************************************
void UdpServerThread (UdpSocket &udpSocket) {
  uint8_t buffer[200];
  printf ("Udp Server Thread started\n");
  bool running {true};

  while (running) {
    int rval = udpSocket.RecvFrom(buffer, sizeof(buffer));
    if (rval > 0) {
       buffer[rval] = 0;
       printf("UDP RX: '%s'\n", buffer);
       int status = udpSocket.Reply(buffer, rval);
       if (status < 0) {
         printf ("UDP TX failed: %s\n", strerror(errno));
       }
    }
  }
  printf ("Ping Thread Exiting\n");
}

  // std::string msg = "stop";
  // udpSocket.SendTo (msg.c_str(), msg.size(), ipAddr, sendToPort);
  // udpSocket.Shutdown(SHUT_RDWR);

// //*****************************************************
// // PongThread
// //*****************************************************
// void PongThread (UdpSocket &udpSocket, std::string ipAddr, std::string sendToPort) {
//   uint8_t buffer[200];

//   printf ("Pong Thread started %s %s\n", ipAddr.c_str(), sendToPort.c_str());

//   if (ipAddr.empty() || sendToPort.empty())
//     return;

//   bool running {true};
//   while (running) {
//     printf ("Pong calling RecvFrom\n");
//     int rval = udpSocket.RecvFrom(buffer, sizeof(buffer), ipAddr, sendToPort);
//     if (rval > 0) {
//        buffer[rval] = 0;
//        printf("Pong: '%s'\n", buffer);
// #if 0       
//        if (udpSocket.SendTo(buffer, rval) <= 0) {
//          printf ("Pong failed to send %s\n", strerror(errno));
//          running = false;
//        }
// #endif       
//      } else {
//         printf ("Pong failed to receive: %s\n", strerror(errno));
//         running = false;
//      }
//      if (std::string((char*)buffer) == std::string("stop")) {
//         printf ("Pong received stop signal\n");
//         running = false;
//      }
     
//   }
//   udpSocket.Shutdown(SHUT_RDWR);
//   printf ("Pong Thread Exiting\n");
// }