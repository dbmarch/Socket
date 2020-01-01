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

  /* 
  * ShutDown
  * @param how   SHUT_RD, SHUT_WR, SHUT_RDWR
  */
  virtual int Shutdown ( int how );

  int SetSockOpt (int level, int optname, const void* optval, socklen_t optlen);
  int GetSockOpt (int level, int optname, void * optval, socklen_t *optlen);


  std::string IpToString (const struct sockaddr * sa);
  std::string Family(int family = -1);   // default to our socket family
  std::string Type(int type = -1 );      // default to our socket type
  std::string Protocol(int protocol );

  protected:
    int sockId{-1};

    int mFamily;

    int mType;
};

#endif