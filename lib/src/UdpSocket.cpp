#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/un.h>
#include "UdpSocket.h"

//*****************************************************
// UdpSocket::UdpSocket
//*****************************************************
UdpSocket::UdpSocket() :  Socket(AF_INET, SOCK_DGRAM) {
  printf ("%s\n", __func__);
  Socket::Open();
}

//*****************************************************
// UdpSocket::UdpSocket
//*****************************************************
UdpSocket::UdpSocket (const UdpSocket& sock) :  Socket(sock) {
}

//*****************************************************
// UdpSocket::~UdpSocket
//*****************************************************
UdpSocket::~UdpSocket() {
  printf ("%s\n", __func__);
}



//*****************************************************
// UdpSocket::SendTo
//*****************************************************
int UdpSocket::SendTo(const uint8_t* buf, size_t len, std::string ipAddr, std::string port) {	
   printf ("%s\n", __func__);
   struct addrinfo *result;
   struct addrinfo fromAddr;
   
   memset(&fromAddr, 0, sizeof(struct addrinfo));
   fromAddr.ai_family = mFamily;    /* Allow IPv4 or IPv6 */
   fromAddr.ai_socktype = mType; 
   fromAddr.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
   fromAddr.ai_protocol = IPPROTO_TCP;    
   fromAddr.ai_canonname = NULL;
   fromAddr.ai_addr = NULL;
   fromAddr.ai_next = NULL;
  
   int status = getaddrinfo(ipAddr.c_str(), port.c_str(), &fromAddr, &result);
   if (status != 0) {
      printf("getaddrinfo: %s\n", gai_strerror(status));
      return 1;
   } else {
      struct addrinfo *rp;
      for (rp = result; rp != NULL; rp = rp->ai_next) {     
          printf("IP:%s %s %s %s\n", 
               IpToString(rp->ai_addr).c_str(),
               Family(rp->ai_family).c_str(), 
               Type(rp->ai_socktype).c_str(), 
               Protocol(rp->ai_protocol).c_str()
               );
      }
   }

   fromAddr.ai_addrlen = result->ai_addrlen;
   fromAddr.ai_addr = result->ai_addr;

   freeaddrinfo(result);

  if (sendto(sockId,  buf, len, 0, fromAddr.ai_addr, fromAddr.ai_addrlen) < 0) {
    printf ("%s failed: %s\n", __func__, strerror(errno));
    return -1;
  }
  return 0;
}

//*****************************************************
// UdpSocket::RecvFrom
//*****************************************************
int UdpSocket::RecvFrom( uint8_t* buf, size_t len, std::string ipAddr, std::string port) {	
   printf ("%s\n", __func__);
   struct addrinfo *result;
   struct addrinfo fromAddr;
   
   memset(&fromAddr, 0, sizeof(struct addrinfo));
   fromAddr.ai_family = mFamily;    /* Allow IPv4 or IPv6 */
   fromAddr.ai_socktype = mType; 
   fromAddr.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
   fromAddr.ai_protocol = IPPROTO_TCP;    
   fromAddr.ai_canonname = NULL;
   fromAddr.ai_addr = NULL;
   fromAddr.ai_next = NULL;
  
   int status = getaddrinfo(ipAddr.c_str(), port.c_str(), &fromAddr, &result);
   if (status != 0) {
      printf("getaddrinfo: %s\n", gai_strerror(status));
      return 1;
   } else {
      struct addrinfo *rp;
      for (rp = result; rp != NULL; rp = rp->ai_next) {     
          printf("IP:%s %s %s %s\n", 
               IpToString(rp->ai_addr).c_str(),
               Family(rp->ai_family).c_str(), 
               Type(rp->ai_socktype).c_str(), 
               Protocol(rp->ai_protocol).c_str()
               );
      }
   }

   fromAddr.ai_addrlen = result->ai_addrlen;
   fromAddr.ai_addr = result->ai_addr;

   freeaddrinfo(result);

  if (recvfrom(sockId,  buf, len, 0, fromAddr.ai_addr, &fromAddr.ai_addrlen) < 0) {
    printf ("%s failed: %s\n", __func__, strerror(errno));
    return -1;
  }
  

  return 0;
}