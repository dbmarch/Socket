#include <string>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "TcpSocket.h"

#include <arpa/inet.h>

int main() {
  TcpSocket sock;
  std::string myAddr("localhost");
  // std::string myAddr("192.168.0.147");
  std::string port("8888");
  printf ("Server\n");


  if (sock.Bind(myAddr, port) < 0) {
    printf ("bind failed %s\n", strerror(errno));
    return 1;
  }

  if (sock.Listen(5) < 0) {
    printf ("bind failed %s\n", strerror(errno));
    return 1;
  }

  TcpSocket *newSocket = sock.Accept();
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
    }
  } else {
    printf ("Accept failed %s\n" , strerror(errno));
  }

  return 0;
}
