#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Socket.h"


//*****************************************************
// Socket::Socket
//*****************************************************
Socket::Socket(int family, int type) :
  mFamily(family), mType (type) {
  printf ("%s\n", __func__);
}


//*****************************************************
// Socket::Socket
//*****************************************************
Socket::Socket (const Socket& sock) {
  sockId = sock.sockId;
  mFamily = sock.mFamily;
  mType = sock.mType;
}

//*****************************************************
// Socket::~Socket
//*****************************************************
Socket::~Socket() {
  printf ("%s\n", __func__);
}

//*****************************************************
// Socket::Open
//*****************************************************
int Socket::Open(void) {	
  printf ("%s\n", __func__);
  sockId = socket(mFamily, mType, 0);
  if (sockId < 0) 
    return sockId;

  int optVal = 1;
  if (setsockopt(sockId, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof (int)) < 0 ) { //You can reuse the address and the port
    printf ("set socket option failed %s\n", strerror(errno));
  }

  return 0;
}


//*****************************************************
// Socket::Close
//*****************************************************
int Socket::Close(void) {	
  int closeId {sockId};
  sockId = -1;
  return close(closeId);  // shutdown is preferred..
}


//*****************************************************
// Socket::Shutdown 
//*****************************************************
int Socket::Shutdown(int how) {	
  return shutdown(sockId, how);
}


//*****************************************************
// Socket::SetSockOpt
//*****************************************************
int Socket::SetSockOpt (int level, int optname, const void* optval, socklen_t optlen) {
    int status = setsockopt (sockId, level, optname, optval, optlen);
    if (status < 0) {
        printf("%s error: %s\n", __func__,strerror(errno));
    }
  return status;
}

//*****************************************************
// Socket::GetSockOpt
//*****************************************************
int Socket::GetSockOpt (int level, int optname, void* optval, socklen_t *optlen) {
    int status = getsockopt (sockId, level, optname, optval, optlen);
    if (status < 0) {
        printf("%s error: %s\n", __func__,strerror(errno));
    }
  return status;
}


//*****************************************************
// Socket::IpToString
//*****************************************************
std::string Socket::IpToString (const struct sockaddr * sa) {
  char ipBuf[INET_ADDRSTRLEN];
  std::string result{};
  switch (sa->sa_family)   {
    case AF_INET:
      inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr),ipBuf, sizeof(ipBuf));
      result = std::string(ipBuf) + std::string(":") + std::to_string(htons(((struct sockaddr_in *)sa)->sin_port));
      break;

    case AF_INET6:
      inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr),ipBuf, sizeof(ipBuf));
      result = std::string(ipBuf) + std::string(":") + std::to_string(htons(((struct sockaddr_in6 *)sa)->sin6_port));
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
std::string Socket::Family(int otherFamily) {
  int family = otherFamily == -1 ? mFamily : otherFamily;
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
std::string Socket::Type(int otherType) {
  int type = otherType == -1 ? mType : otherType;
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
std::string Socket::Protocol(int protocol) {
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