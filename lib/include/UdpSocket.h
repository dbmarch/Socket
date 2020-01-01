#ifndef __TCPSOCKET_H
#define __TCPSOCKET_H

#include <string>
#include <netinet/in.h>
#include "Socket.h"
 
class UdpSocket : public Socket {
public:
  UdpSocket();
  UdpSocket (const UdpSocket& s);

  virtual ~UdpSocket();
  
  int SendTo ( const uint8_t * buf, size_t len, std::string ipAddr, std::string port );

  int RecvFrom( uint8_t * buf, size_t len, std::string ipAddr, std::string port);
};

#endif