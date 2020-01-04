#ifndef __SOCKET_H
#define __SOCKET_H

#include <vector>
#include <string>
#include <stdint.h>
#include<sys/socket.h> 

class Socket {
public:
  Socket(int domain, int type, int protocol);
  Socket (const Socket& sock);

  virtual ~Socket();

  virtual int Open ( void) ;
  virtual int Close( void ); 

  /* 
  * ShutDown
  * @param how   SHUT_RD, SHUT_WR, SHUT_RDWR
  */
  virtual int Shutdown ( int how );

  /* 
  * SetSockOpt
  * @param how   SHUT_RD, SHUT_WR, SHUT_RDWR
  */
  int SetSockOpt (int level, int optname, const void* optval, socklen_t optlen);
  int GetSockOpt (int level, int optname, void * optval, socklen_t *optlen);

  virtual void EnableDebug(bool enableIt);

  /* 
  * Select
  * @param Vectors of sockets for read, write and exceptions.
  * @param timeout
  * 
  * @return vectors are updated with the only the sockets that have activity (Same as select)
  */
  static int Select ( std::vector<Socket> *readfds, std::vector<Socket> *writefds, std::vector<Socket> *exceptfds, struct timeval & timeout);


  int SockId () const  { return sockId; }
  int Family() const   { return mFamily; }
  int Type () const    { return mType; }
  int Protocol() const {return mProtocol; }

  std::string IpToString (const struct sockaddr * sa) const;
  std::string AddrToString (const struct sockaddr * sa) const;

  std::string FamilyToString (int family ) const;
  std::string TypeToString (int type ) const;      // default to our socket type
  std::string ProtocolToString (int protocol ) const;

  protected:
    int sockId{-1};

    int mFamily{AF_INET};

    int mType{SOCK_RAW};

    int mProtocol { IPPROTO_IP };

    bool mDebug{false};
};

#endif