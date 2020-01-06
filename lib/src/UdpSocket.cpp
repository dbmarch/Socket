#include <cstring>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <netinet/ip.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/un.h>
#include "UdpSocket.h"

//*****************************************************
// UdpSocket::UdpSocket
//*****************************************************
UdpSocket::UdpSocket() : Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) {
  if (mDebug) printf ("%s\n", __func__);
  Socket::Open();
  std::memset(&mLastReceivedAddr, 0, sizeof(mLastReceivedAddr));
}


//*****************************************************
// UdpSocket::~UdpSocket
//*****************************************************
UdpSocket::~UdpSocket() {
  if (mDebug) printf ("%s\n", __func__);
  Close();
}


//*****************************************************
// UdpSocket::Bind
//*****************************************************
int UdpSocket::Bind(std::string ipAddr, std::string port) {
   if (mDebug) printf ("%s\n", __func__);
   struct addrinfo *result;
   struct addrinfo server;

   mIpAddr = ipAddr;
   mPort = port;

   memset(&server, 0, sizeof(struct addrinfo));
   server.ai_family = mFamily;    /* Allow IPv4 or IPv6 */
   server.ai_socktype = mType; 
   server.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
   server.ai_protocol = 0;          /* Any protocol */
   server.ai_canonname = NULL;
   server.ai_addr = NULL;
   server.ai_next = NULL;
  
   int status = getaddrinfo(ipAddr.c_str(), port.c_str(), &server, &result);
   if (status != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
      return -1;
   } else {
      struct addrinfo *rp;
      for (rp = result; rp != NULL; rp = rp->ai_next) {     
          printf("BIND IP:%s %s %s %s\n", 
               IpToString(rp->ai_addr).c_str(),
               FamilyToString(rp->ai_family).c_str(), 
               TypeToString(rp->ai_socktype).c_str(), 
               ProtocolToString(rp->ai_protocol).c_str()
               );
      }
   }
   server.ai_addrlen = result->ai_addrlen;
   server.ai_addr = result->ai_addr;

   freeaddrinfo(result);

   return bind(mSocket, server.ai_addr, server.ai_addrlen);
  }



//*****************************************************
// UdpSocket::SendTo
//*****************************************************
int UdpSocket::SendTo ( const char * buf, size_t len, std::string ipAddr, std::string port){
  return SendTo( reinterpret_cast<const uint8_t*> (buf), len, ipAddr, port);
}

//*****************************************************
// UdpSocket::SendTo
//*****************************************************
int UdpSocket::SendTo(const uint8_t* buf, size_t len, std::string ipAddr, std::string port) {	
   if (mDebug) printf ("%s\n", __func__);

   std::string sendIp   = ipAddr.empty() ? mIpAddr : ipAddr;
   std::string sendPort = port.empty() ? mPort : port;

   if (sendIp.empty() || sendPort.empty()) {
     fprintf (stderr, "%s failed:  Require ipAddr(%s) & port(%s)\n", __func__, sendIp.c_str(), sendPort.c_str());
     return -1;
   }
   struct addrinfo *result;
   struct addrinfo fromAddr;

   memset(&fromAddr, 0, sizeof(struct addrinfo));
   fromAddr.ai_family = mFamily;    /* Allow IPv4 or IPv6 */
   fromAddr.ai_socktype = mType; 
   fromAddr.ai_flags = 0;    
   fromAddr.ai_protocol = IPPROTO_UDP;    
   fromAddr.ai_canonname = NULL;
   fromAddr.ai_addr = NULL;
   fromAddr.ai_next = NULL;
  
   int status = getaddrinfo(sendIp.c_str(), sendPort.c_str(), &fromAddr, &result);
   if (status != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
      return -1;
   } else {
      struct addrinfo *rp;
      for (rp = result; rp != NULL; rp = rp->ai_next) {     
          if (mDebug) printf("SENDTO IP:%s %s %s %s\n", 
               IpToString(rp->ai_addr).c_str(),
               FamilyToString(rp->ai_family).c_str(), 
               TypeToString(rp->ai_socktype).c_str(), 
               ProtocolToString(rp->ai_protocol).c_str()
               );
      }
   }

   fromAddr.ai_addrlen = result->ai_addrlen;
   fromAddr.ai_addr = result->ai_addr;

   freeaddrinfo(result);

  int rval = sendto(mSocket,  buf, len, 0, fromAddr.ai_addr, fromAddr.ai_addrlen);
  if (rval < 0) {
    fprintf (stderr, "%s failed: %s\n", __func__, strerror(errno));
  }
  return rval;
}


//*****************************************************
// UdpSocket::RecvFrom
//*****************************************************
int UdpSocket::RecvFrom( char * buf, size_t len, std::string ipAddr, std::string port){
  return RecvFrom( reinterpret_cast<uint8_t*> (buf), len, ipAddr,port);
}

//*****************************************************
// UdpSocket::RecvFrom
//*****************************************************
int UdpSocket::RecvFrom( uint8_t* buf, size_t len, std::string ipAddr, std::string port) {	
   std::string recvIp   = ipAddr.empty() ? mIpAddr : ipAddr;
   std::string recvPort = port.empty() ? mPort : port;

   if (recvIp.empty() || recvPort.empty()) {
     fprintf (stderr, "%s failed:  Require ipAddr(%s) & port(%s)\n", __func__, recvIp.c_str(), recvPort.c_str());
     return -1;
   }

   if (mDebug) printf("%s %s %s \n", __func__, ipAddr.c_str(), port.c_str());
   
   struct addrinfo *result;
   struct addrinfo fromAddr;
   
   memset(&fromAddr, 0, sizeof(struct addrinfo));
   fromAddr.ai_family = mFamily;    /* Allow IPv4 or IPv6 */
   fromAddr.ai_socktype = mType; 
   fromAddr.ai_flags = 0;    /* For wildcard IP address */
   fromAddr.ai_protocol = IPPROTO_UDP;    
   fromAddr.ai_canonname = NULL;
   fromAddr.ai_addr = NULL;
   fromAddr.ai_next = NULL;
  
   int status = getaddrinfo(recvIp.c_str(), recvPort.c_str(), &fromAddr, &result);
   if (status != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
      return -1;
   } else {
      struct addrinfo *rp;
      for (rp = result; rp != NULL; rp = rp->ai_next) {     
          if (mDebug) printf("RECVFROM IP:%s %s %s %s\n", 
               IpToString(rp->ai_addr).c_str(),
               FamilyToString(rp->ai_family).c_str(), 
               TypeToString(rp->ai_socktype).c_str(), 
               ProtocolToString(rp->ai_protocol).c_str()
               );
      }
   }

   fromAddr.ai_addrlen = result->ai_addrlen;
   fromAddr.ai_addr = result->ai_addr;

   freeaddrinfo(result);
  
  int rval = recvfrom(mSocket,  buf, len, 0, fromAddr.ai_addr, &fromAddr.ai_addrlen);
  if ( rval < 0) {
    fprintf (stderr, "%s failed: %s\n", __func__, strerror(errno));
  } else{
    std::memcpy (&mLastReceivedAddr, &fromAddr, sizeof(fromAddr));
    if (mDebug) printf("RECVFROM SUCCESS: %d bytes from %s\n", rval, IpToString(fromAddr.ai_addr).c_str());
  }

  return rval;
}



//*****************************************************
// UdpSocket::Reply
//*****************************************************
int UdpSocket::Reply ( const char * buf, size_t len){
  return Reply( reinterpret_cast<const uint8_t*> (buf), len);
}

//*****************************************************
// UdpSocket::Reply
//*****************************************************
int UdpSocket::Reply(const uint8_t* buf, size_t len) {	
   if (mDebug) printf ("%s\n", __func__);

   if (mLastReceivedAddr.ai_addrlen == 0) {
     fprintf (stderr, "%s failed:  Invalid Last Received IP %s\n", __func__, IpToString(mLastReceivedAddr.ai_addr).c_str());
     return -1;

   }
  int rval = sendto(mSocket,  buf, len, 0, mLastReceivedAddr.ai_addr, mLastReceivedAddr.ai_addrlen);
  if (rval < 0) {
    fprintf (stderr, "%s failed: %s\n", __func__, strerror(errno));
  }
  return rval;
}

