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

/* 
  * Timed Receive
  * @param buf   Buffer made of unsigned bytes
  * @param len   Buffer Size
  * @param t     wait time
  * @return 0 for timeout, -1 for error, # for bytes received
  * 
  * @return vectors are updated with the only the sockets that have activity (Same as select)
  */
  int Recv (uint8_t* buf, size_t len, timeval &t);   
/* 
  * Timed Receive
  * @param buf   Buffer made of char array
  * @param len   Buffer Size
  * @param t     wait time
  * @return 0 for timeout, -1 for error, # for bytes received
  * 
  * @return vectors are updated with the only the sockets that have activity (Same as select)
  */
  int Recv (char* buf, size_t len, timeval &t);   

  int Send( const uint8_t * buf, size_t len);
  int Send( const char*  buf, size_t len);
  int Send(const std::string msg);

  void EnableDebug (bool enableIt) { Socket::EnableDebug(enableIt); }

  sockaddr    GetPeerSockAddr() const {return peer_addr;}
  std::string GetPeerIp() const { return IpToString(&peer_addr); }

protected:

  void AcceptedSocket(int sockId);

  struct sockaddr peer_addr;

};

#endif