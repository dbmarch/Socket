#include <thread>
#include <mutex>
#include <vector>
#include <string>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "TcpSocket.h"

#include <arpa/inet.h>



class MessageHandler {
  public:
    MessageHandler() {};
    ~MessageHandler(){};
    

   void ReceiveThread(TcpSocket &newSocket) {
      printf ("Receive Thread started\n");
      uint8_t buffer[2000];
      bool receiving{true};
      while (receiving) {
        int rval = newSocket.Recv(buffer, sizeof(buffer));
        if (rval > 0) {
    			buffer[rval] = 0;
    			printf("received: %s\n", buffer);
          std::string serverMessage= std::string ("Received your message: ") + std::string ("'") + std::string((char*) buffer) + std::string ("'");
          newSocket.Send((uint8_t*) serverMessage.c_str(), serverMessage.length());
          newSocket.Shutdown(SHUT_RDWR);
        } else if (rval < 0) {
            printf("Receive Error: %s\n", strerror(errno));
            receiving = false;
         } else if (rval == 0) {
            printf("0 bytes\n");
            receiving = false;
         }
      }
      printf ("Receive Thread Exiting\n");
   }

 void AcceptThread (TcpSocket &listenSocket) {
    printf ("AcceptThread started.\n");
    while (1)
    {
      TcpSocket *newSocket = listenSocket.Accept();
      if (newSocket) {
        printf ("Accepted %s\n", newSocket->GetPeerAddr().c_str());
        std::thread receive (&MessageHandler::ReceiveThread, this, std::ref(*newSocket));
        receive.join();
       } else {
        printf ("Accept failed %s\n" , strerror(errno));
        break;
      }
    }
  }

 void ProcessMessages(TcpSocket &listenSocket) {
    std::thread accept (&MessageHandler::AcceptThread, this,  std::ref(listenSocket));
    accept.join();
    printf ("ProcessMessages Exiting\n");
 }

 private:
};


//*****************************************************
// main
//*****************************************************
int main() {
  TcpSocket listenSocket;
  std::string myAddr("localhost");
  std::string port("8888");
  printf ("Server2\n");
  MessageHandler m;

  if (listenSocket.Bind(myAddr, port) < 0) {
    printf ("bind failed %s\n", strerror(errno));
    return 1;
  }

  if (listenSocket.Listen(5) < 0) {
    printf ("bind failed %s\n", strerror(errno));
    return 1;
  }

  m.ProcessMessages(std::ref(listenSocket));
  
  printf ("Exiting\n");
  listenSocket.Shutdown(SHUT_RDWR);
  return 0;
}

