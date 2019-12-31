#ifndef __SOCKET_H
#define __SOCKET_H

#include <string>
#include <stdint.h>
#include<sys/socket.h> 

class Socket {
public:
  Socket(int domain, int type);
  Socket (const Socket& sock);

  virtual ~Socket();

  virtual int Open ( void) ;
  virtual int Close( void ); 

  virtual int Recv (uint8_t * buf , size_t len);
  virtual int Send (uint8_t * buf , size_t len);

  /* 
  * ShutDown
  * @param how   (SHUT_RD, SHUT_WR, SHUT_RDWR)
  */
  virtual int Shutdown ( int how );

  std::string GetIpString (const struct sockaddr * sa);
  std::string Family(int family = -1);   // default to our socket family
  std::string Type(int type = -1 );      // default to our socket type
  std::string Protocol(int protocol );

  protected:
    int sockId{-1};

    int mFamily;

    int mType;
};

#endif