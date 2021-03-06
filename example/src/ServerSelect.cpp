#include <thread>

#include <string>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "TcpSocket.h"

#include <arpa/inet.h>


using ReceiveFunction = void (*)(TcpSocket&);
static void AcceptThread (std::string name, TcpSocket &listenSocket, ReceiveFunction receiveFunction);
static void ReceiveThread (TcpSocket &newSocket);

//*****************************************************
// main
//*****************************************************
int main() {
  TcpSocket listenSocket;
  std::string myAddr("localhost");
  std::string port("8888");
  printf ("Server2\n");

  listenSocket.EnableDebug(false);

  if (listenSocket.Bind(myAddr, port) < 0) {
    printf ("bind failed %s\n", strerror(errno));
    return 1;
  }

  if (listenSocket.Listen(5) < 0) {
    printf ("bind failed %s\n", strerror(errno));
    return 1;
  }

  // typically one would use select on the listen socket.  Then we would spawn new threads.
  // This code probably breaks when both threads exit at the same time and we attempt to accept on both threads.
  
  std::thread accept1 (AcceptThread, std::string("1"), std::ref(listenSocket), std::ref(ReceiveThread));
{

    printf ("Client\n");
    TcpSocket sock;

    std::string ipAddr ("localhost");
    std::string port ("8888");

    sock.Connect(ipAddr, port);

    while (1) {}
    }
  accept1.join();
  
  printf ("Exiting\n");
  
  return 0;
}

//*****************************************************
// AcceptThread
//*****************************************************
void AcceptThread( std::string name, TcpSocket &listenSocket, ReceiveFunction receiveFunction) {
  printf ("AcceptThread started...%s\n", name.c_str());
  while (1)
  {
    std::vector<Socket*> readSockList;
    readSockList.push_back(&listenSocket);
    timeval t;
    t.tv_sec = 3;
    t.tv_usec = 0;
    int status = Socket::Select(&readSockList, nullptr , nullptr, t );
    if (status == 0) {
      printf ("Waiting...\n");
    } else if (status < 0) {
      printf ("Select Error %s\n", strerror (errno));
      break;
    } else {
      printf ("Activity...%d\n", status);
      TcpSocket *newSocket = listenSocket.Accept(); // This will return
      if (newSocket) {
        printf ("%s Accepted!\n", name.c_str());
        std::thread receive (receiveFunction, std::ref(*newSocket));
        receive.join();
    
      } else {
        printf ("Accept failed %s\n" , strerror(errno));
        break;
      }
    }
  }
}


//*****************************************************
// ReceiveThread
//*****************************************************
void ReceiveThread (TcpSocket &newSocket) {
  printf ("Receive Thread started\n");
  uint8_t buffer[2000];
    while (1) {
      
      timeval t;
      t.tv_sec = 5;
      t.tv_usec  = 0;
      int rval = newSocket.Recv(buffer, sizeof(buffer), t);

      if (rval < 0) {
          printf("Receive Error: %s\n", strerror(errno));
          break;
       } else if (rval == 0) {
          printf("timeout\n");
          break;
       } else {
         buffer[rval] = 0;
         printf("received: %s\n", buffer);
       }
       std::string serverMessage= std::string ("Received your message: ") + std::string ("'") + std::string((char*) buffer) + std::string ("'");
       newSocket.Send((uint8_t*) serverMessage.c_str(), serverMessage.length());

       newSocket.Shutdown(SHUT_RDWR);
    }
    printf ("Receive Thread Exiting\n");
}