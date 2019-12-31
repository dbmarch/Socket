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

  int Connect( const char* ipAddr, uint16_t port);

  int Connect( uint16_t port);
  
  TcpSocket* Accept();

  int Bind (in_addr_t ipAddr, uint16_t port);

  int Listen (int backlog) ;
  
  int Recv( uint8_t * buf, size_t len) { return Socket::Recv(buf,len);}

  int Send( uint8_t * buf, size_t len) { return Socket::Send(buf,len);}

  int Send(std::string msg);

protected:

  void AcceptedSocket(int sockId);

  
};

#endif