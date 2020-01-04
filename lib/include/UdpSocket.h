#ifndef __TCPSOCKET_H
#define __TCPSOCKET_H

#include <string>
#include <netinet/in.h>
#include <netdb.h>
#include "Socket.h"
 
class UdpSocket : public Socket {
public:
  UdpSocket();
  UdpSocket (const UdpSocket& s);

  virtual ~UdpSocket();

  int Bind(std::string ipAddr, std::string port);

  int RecvFrom( uint8_t * buf, size_t len, std::string ipAddr=std::string(), std::string port=std::string());
  int RecvFrom( char * buf, size_t len, std::string ipAddr=std::string(), std::string port=std::string());

  int SendTo ( const uint8_t * buf, size_t len, std::string ipAddr=std::string(), std::string port=std::string());
  int SendTo ( const char * buf, size_t len, std::string ipAddr=std::string(), std::string port=std::string());

  // Send a message to the last received addr/port
  int Reply ( const uint8_t * buf, size_t len);
  int Reply ( const char * buf, size_t len);

  void EnableDebug (bool enableIt) { Socket::EnableDebug(enableIt); }

  

protected:
  std::string mIpAddr{};              // Use this if we don't supply an ip
  std::string mPort{};                // Use this if we don't supply a port
  
  struct addrinfo mLastReceivedAddr;    // Use this for replying
  
};

#endif