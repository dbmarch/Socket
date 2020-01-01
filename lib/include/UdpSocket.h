#ifndef __TCPSOCKET_H
#define __TCPSOCKET_H

#include <string>
#include <netinet/in.h>
#include "Socket.h"
 
class UdpSocket : public Socket {
public:
  UdpSocket();
  UdpSocket(std::string ipAddr, std::string port);
  UdpSocket (const UdpSocket& s);

  virtual ~UdpSocket();
  
  int SendTo ( const uint8_t * buf, size_t len, std::string ipAddr = std::string(), std::string port =std::string());
  int SendTo ( const char * buf, size_t len, std::string ipAddr = std::string(), std::string port =std::string());

  int RecvFrom( uint8_t * buf, size_t len, std::string ipAddr=std::string(), std::string port= std::string());
  int RecvFrom( char * buf, size_t len, std::string ipAddr=std::string(), std::string port= std::string());

  void EnableDebug (bool enableIt) { Socket::EnableDebug(enableIt); }


protected:
  std::string mIpAddr;
  std::string mPort;
};

#endif