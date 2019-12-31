#include <string>
#include <stdio.h>
#include "TcpSocket.h"

int main() {
    printf ("Client\n");
    TcpSocket sock;
    std::string ipAddr ("localhost");
    std::string port ("8888");

    sock.Connect(ipAddr, port);

    std::string msg("Hello\n");
    sock.Send( (uint8_t*)msg.c_str(), msg.length());

    return 0;
}


