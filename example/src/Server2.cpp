#include <thread>

#include <string>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "TcpSocket.h"

#include <arpa/inet.h>


static void AcceptThread(TcpSocket &listenSocket);


//*****************************************************
// main
//*****************************************************
int main() {
  TcpSocket listenSocket;
  std::string myAddr("localhost");
  std::string port("8888");
  printf ("Server2\n");


  if (listenSocket.Bind(myAddr, port) < 0) {
    printf ("bind failed %s\n", strerror(errno));
    return 1;
  }

  if (listenSocket.Listen(5) < 0) {
    printf ("bind failed %s\n", strerror(errno));
    return 1;
  }

  std::thread accept (AcceptThread, std::ref(listenSocket));
  accept.join();
  printf ("Exiting\n");
  
  return 0;
}

//*****************************************************
// AcceptThread
//*****************************************************
void AcceptThread( TcpSocket &listenSocket) {
  printf ("AcceptThread started...\n");
  while (1)
  {
  TcpSocket *newSocket = listenSocket.Accept();
    if (newSocket) {
      printf ("Accepted!\n");

      uint8_t buffer[2000];
      while (1) {
        int rval = newSocket->Recv(buffer, sizeof(buffer));

        if (rval < 0) {
            printf("Receive Error: %s\n", strerror(errno));
            break;
         } else if (rval == 0) {
            printf("0 bytes\n");
            break;
         } else {
           buffer[rval] = 0;
           printf("received: %s\n", buffer);
         }
         std::string serverMessage= std::string ("Received your message: ") + std::string ("'") + std::string((char*) buffer) + std::string ("'");
         newSocket->Send((uint8_t*) serverMessage.c_str(), serverMessage.length());

         newSocket->Shutdown(SHUT_RDWR);
      }
    } else {
      printf ("Accept failed %s\n" , strerror(errno));
      break;
    }
  }
}