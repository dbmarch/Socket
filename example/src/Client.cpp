#include <string>
#include <stdio.h>
#include "TcpSocket.h"

int main() {
    printf ("Client\n");
    TcpSocket sock;

    std::string ipAddr ("localhost");
    std::string port ("8888");

    sock.Connect(ipAddr, port);

    std::string msg("Client Socket says hello.");
    sock.Send( (uint8_t*)msg.c_str(), msg.length());

    uint8_t buf[100];
    if (sock.Recv(buf, sizeof(buf)) > 0)
    {
      printf ("Server Says: %s\n", buf);
    }
    return 0;
}


