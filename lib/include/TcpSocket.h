#ifndef __TCPSOCKET_H
#define __TCPSOCKET_H

#include <string>
#include <netinet/in.h>
#include "Socket.h"
 
class TcpSocket : public Socket {
public:
  TcpSocket();
  TcpSocket (const TcpSocket& s);

  virtual ~TcpSocket();

  int Connect( std::string ipAddr, std::string port);

  int Connect( uint16_t port);
  
  TcpSocket* Accept();
  std::string GetPeerAddr();

  int Bind (std::string ipAddr, std::string port);

  int Listen (int backlog) ;
  
  int Recv( uint8_t * buf, size_t len);
  int Recv( char * buf, size_t len);

  int Send( const uint8_t * buf, size_t len);
  int Send( const char*  buf, size_t len);
  int Send(const std::string msg);

  void EnableDebug (bool enableIt) { Socket::EnableDebug(enableIt); }
protected:

  void AcceptedSocket(int sockId);

  struct sockaddr peer_addr;

};

#endif