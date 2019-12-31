#ifndef __SOCKET_H
#define __SOCKET_H

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

  virtual int Shutdown ( int how );

  protected:
    int sockId{-1};

    int mDomain;

    int mType;
};

#endif