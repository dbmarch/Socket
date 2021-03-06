#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include "Socket.h"


//*****************************************************
// Socket::Socket
//*****************************************************
Socket::Socket(int family, int type, int protocol) :
  mFamily(family), mType (type), mProtocol(protocol) {
  if (mDebug) printf ("%s\n", __func__);
}


//*****************************************************
// Socket::~Socket
//*****************************************************
Socket::~Socket() {
  if (mDebug) printf ("%s\n", __func__);
}

//*****************************************************
// Socket::Open
//*****************************************************
int Socket::Open(void) {	
  if (mDebug) printf ("%s\n", __func__);
  mSocket = socket(mFamily, mType, 0);
  if (mSocket < 0) 
    return mSocket;

  int optVal = 1;
  if (setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof (int)) < 0 ) { //You can reuse the address and the port
    fprintf (stderr,"set socket option failed %s\n", strerror(errno));
  }

  return 0;
}


//*****************************************************
// Socket::Close
//*****************************************************
int Socket::Close(void) {	
  if (mDebug) printf ("Closing %d\n",mSocket);
  int closeId {mSocket};
  if (mSocket != -1) {
    mSocket = -1;
    return close(closeId);  // shutdown is preferred..
  }
  return 0;
}


//*****************************************************
// Socket::Shutdown 
//*****************************************************
int Socket::Shutdown(int how) {	
  return shutdown(mSocket, how);
}


//*****************************************************
// Socket::EnableDebug
//*****************************************************
void Socket::EnableDebug(bool enableIt) { 
  mDebug = enableIt; 
 };
  

//*****************************************************
// Socket::SetSockOpt
//*****************************************************
int Socket::SetSockOpt (int level, int optname, const void* optval, socklen_t optlen) {
    int status = setsockopt (mSocket, level, optname, optval, optlen);
    if (status < 0) {
        fprintf(stderr,"%s error: %s\n", __func__,strerror(errno));
    }
  return status;
}

//*****************************************************
// Socket::GetSockOpt
//*****************************************************
int Socket::GetSockOpt (int level, int optname, void* optval, socklen_t *optlen) {
    int status = getsockopt (mSocket, level, optname, optval, optlen);
    if (status < 0) {
        fprintf(stderr,"%s error: %s\n", __func__,strerror(errno));
    }
  return status;
}

//*****************************************************
// Socket::GetSockOpt
//*****************************************************
int Socket::Select (std::vector<Socket*> *readSockList, std::vector<Socket*> *writeSockList, std::vector<Socket*> *exceptSockList, struct timeval & timeout) {
  fd_set rfds, wfds, efds;
  int nfds{0}; // 1 more than the maximum of any file descriptor in the set.

  // Clear them
  FD_ZERO (&rfds);
  FD_ZERO (&wfds);
  FD_ZERO (&efds);

  // load our socket ids
  if (readSockList) {
    for (auto const& socket : *readSockList) {
       FD_SET (socket->SockId(), &rfds);
       nfds = socket->SockId() > nfds ? socket->SockId() : nfds;
    }
  }
  if(writeSockList) {
    for (auto const & socket : *writeSockList) {
       FD_SET (socket->SockId(), &wfds);
       nfds = socket->SockId() > nfds ? socket->SockId() : nfds;
    }
  }

  if(exceptSockList) {
    for (auto const & socket : *exceptSockList) {
       FD_SET (socket->SockId(), &efds);
       nfds = socket->SockId() > nfds ? socket->SockId() : nfds;
    }
  }
  ++ nfds;
   
  // int status = select (nfds, &rfds, &wfds, &efds, &timeout);
  int status = select (nfds, &rfds, nullptr, nullptr, &timeout);
  if (status <= 0) {
    return status;
  } else {
    if (readSockList) {
      for (auto socket = readSockList->begin() ; socket != readSockList->end() ; ++socket) {
         if (!FD_ISSET ((*socket)->SockId(), &rfds)){
            readSockList->erase(socket);
         } 
      }
    }
    if (writeSockList) {
      for (auto socket = writeSockList->begin() ; socket != writeSockList->end() ; ++socket) {
         if (!FD_ISSET ((*socket)->SockId(), &wfds)) {
           writeSockList->erase(socket);
         }
      }
    }
    if (exceptSockList) {
      for (auto socket = exceptSockList->begin() ; socket != exceptSockList->end() ; ++socket) {
         if (!FD_ISSET ((*socket)->SockId(), &efds)) {
           exceptSockList->erase(socket);
         }
      }
    }
  }
  return status;
}


//*****************************************************
// Socket::IpToString
//*****************************************************
std::string Socket::IpToString (const struct sockaddr * sa) const {
  char ipBuf[INET_ADDRSTRLEN];
  std::string result{};
  switch (sa->sa_family)   {
    case AF_INET:
      inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr),ipBuf, sizeof(ipBuf));
      result = std::string(ipBuf);
      break;

    case AF_INET6:
      inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr),ipBuf, sizeof(ipBuf));
      result = std::string(ipBuf);
      break;

    case AF_UNSPEC:
      result = std::string("NONE");
      break;

    default:
      result = std::string("???");
  }
  return result;
}


//*****************************************************
// Socket::IpToString
//*****************************************************
std::string Socket::AddrToString (const struct sockaddr * sa) const {
  std::string ip = IpToString(sa);

  std::string result{};
  switch (sa->sa_family)   {
    case AF_INET:
      result = ip + std::string(":") + std::to_string(htons(((struct sockaddr_in *)sa)->sin_port));
      break;

    case AF_INET6:
      result = ip + std::string(":") + std::to_string(htons(((struct sockaddr_in6 *)sa)->sin6_port));
      break;

    case AF_UNSPEC:
      result = std::string("NONE");
      break;

    default:
      result = std::string("???");
  }
  return result;
}

//*****************************************************
// Socket::Family
//*****************************************************
std::string Socket::FamilyToString(int family) const {

  switch (family)   {
    case AF_INET:  return std::string ("AF_INET");
    case AF_INET6: return std::string ("AF_INET6");
    case AF_LOCAL: return std::string ("AF_LOCAL");
    default:       return std::string ("AF_???");
  }
  return std::string ("???");
}


//*****************************************************
// Socket::Type
//*****************************************************
std::string Socket::TypeToString(int type) const {
  switch (type)   {
    case SOCK_STREAM: return std::string ("SOCK_STREAM");
    case SOCK_DGRAM:  return std::string ("SOCK_DGRAM");
    case SOCK_RAW:    return std::string ("SOCK_RAW");
    default:          return std::string ("SOCK_???");
  }
  return std::string ("???");
}


//*****************************************************
// Socket::Protocol
//*****************************************************
std::string Socket::ProtocolToString (int protocol) const {
  switch (protocol) {
    case IPPROTO_IP:    return std::string ("IPPROTO_IP");
    case IPPROTO_ICMP:  return std::string ("IPPROTO_ICMP");
    case IPPROTO_IGMP:  return std::string ("IPPROTO_IGMP");
    case IPPROTO_TCP:   return std::string ("IPPROTO_TCP");
    case IPPROTO_UDP:   return std::string ("IPPROTO_UDP");
    case IPPROTO_IPV6:  return std::string ("IPPROTO_IPV6");
    case IPPROTO_RAW:   return std::string ("IPPROTO_RAW");
    default:            return std::string ("IPPROTO_???");
  }
  return std::string ("???");
}