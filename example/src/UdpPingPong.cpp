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

static void UdpServerThread (UdpSocket &newSocket, std::string ipAddr, std::string responsePort);

class PingPongTest {
public:
    PingPongTest( int iterations,  std::string serverIp,  std::string serverPort) :
      mIterations{iterations}, mServerIp{serverIp} ,mServerPort{serverPort} {

    };
    ~PingPongTest(){};
    

   void ReceiveThread(UdpSocket &udpSocket) {
      printf ("PingPongTest Receive Thread started\n");
      char buffer[200];
      bool receiving{true};
      while (receiving) {
        int rval = udpSocket.RecvFrom(buffer, sizeof(buffer));
        if (rval > 0) {
          const std::lock_guard<std::mutex> lock(mMutex);

    			buffer[rval] = 0;
          std::string msg{buffer};
          if (mDebug) printf ("Put %s in MQ\n", msg.c_str());
          mMQ.push_back(std::move(msg));
        } else {
            fprintf(stderr, "Receive Error: %s\n", strerror(errno));
            receiving = false;
         } 
      }
      printf ("Receive Thread Exiting\n");
   }

 void MainThread (UdpSocket &udpSocket) {
    printf ("Udp Test Main Thread  started.\n");
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
            timeoutCounter = 0;
          }
      }
      if (timeoutCounter && waitingReply) {
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
      if (mIterations == counter)  {
        std::string msg = "stop";
        udpSocket.SendTo(msg.c_str(), msg.size(), mServerIp, mServerPort);
        break;
      }
        
      std::string msg = "UDP TX # " + std::to_string(counter);
      printf ("%s\n", msg.c_str());
      if (udpSocket.SendTo(msg.c_str(), msg.size(), mServerIp, mServerPort)< 0) {
        fprintf (stderr,"Udp Failed to send packet %s\n", strerror(errno));
        running = false;
      } else {
        waitingReply = true;
        counter++;
        timeoutCounter = 100;
      }
    }
  }
  printf ("main thread exiting\n");
}

 void RunPingPongTest(UdpSocket &udpSocket) {
    std::thread receive (&PingPongTest::ReceiveThread, this,  std::ref(udpSocket));
    std::thread main (&PingPongTest::MainThread, this,  std::ref(udpSocket));
    main.join();
    receive.join();
    printf ("ProcessMessages Exiting\n");
 }

protected:
  int mIterations;
  std::string mServerIp;
  std::string mServerPort;

  std::mutex mMutex;
  std::vector<std::string> mMQ{};
  bool mDebug {false};
};



//*****************************************************
// main
//*****************************************************
int main() {
  std::string serverIp("localhost");
  std::string serverPort("8900");
  
  std::string responseIp ("localhost");
  std::string responsePort("8901");
  
  UdpSocket udpSocket;
  udpSocket.EnableDebug(false);
  
  udpSocket.Bind (serverIp, serverPort);

  UdpSocket udpReponse; 
  udpReponse.Bind (responseIp, responsePort);

  
  std::thread udpServerThread (UdpServerThread, std::ref(udpSocket), responseIp, responsePort);
  
  PingPongTest pingpong (100, serverIp, serverPort);

  pingpong.RunPingPongTest(udpReponse);

  udpServerThread.join();
  
  printf ("Exiting\n");
  return 0;
}


//*****************************************************
// UdpServerThread
//*****************************************************
void UdpServerThread (UdpSocket &udpSocket, std::string responseIp, std::string responsePort) {
  char buffer[200];
  printf ("Udp Server Thread started\n");
  bool running {true};

  while (running) {
    int rval = udpSocket.RecvFrom(buffer, sizeof(buffer));
    if (rval > 0) {
       buffer[rval] = 0;
       if (std::string(buffer) == std::string("stop")){
         break;
       }

       printf("SRVR UDP ECHO: '%s'\n", buffer);
       int status = udpSocket.SendTo(buffer, rval, responseIp, responsePort);
       if (status < 0) {
         printf ("UDP TX failed: %s\n", strerror(errno));
       }
    }
  }
  printf ("Ping Thread Exiting\n");
}
