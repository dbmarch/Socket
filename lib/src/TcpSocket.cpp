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
#include "TcpSocket.h"

//*****************************************************
// TcpSocket::TcpSocket
//*****************************************************
TcpSocket::TcpSocket() :  Socket(AF_INET, SOCK_STREAM) {
  printf ("%s\n", __func__);
  Socket::Open();
}

//*****************************************************
// TcpSocket::TcpSocket
//*****************************************************
TcpSocket::TcpSocket (const TcpSocket& sock) :  Socket(sock) {
}

//*****************************************************
// TcpSocket::~TcpSocket
//*****************************************************
TcpSocket::~TcpSocket() {
  printf ("%s\n", __func__);
}



//*****************************************************
// TcpSocket::Bind
//*****************************************************
int TcpSocket::Bind(std::string ipAddr, std::string port) {
   printf ("%s\n", __func__);
   struct addrinfo *result;
   struct addrinfo server;

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
      printf("getaddrinfo: %s\n", gai_strerror(status));
      return 1;
   } else {
      struct addrinfo *rp;
      for (rp = result; rp != NULL; rp = rp->ai_next) {     
          printf("IP:%s %s %s %s\n", 
               GetIpString(rp->ai_addr).c_str(),
               Family(rp->ai_family).c_str(), 
               Type(rp->ai_socktype).c_str(), 
               Protocol(rp->ai_protocol).c_str()
               );
      }
   }
   server.ai_addrlen = result->ai_addrlen;
   server.ai_addr = result->ai_addr;

   freeaddrinfo(result);

   return bind(sockId, server.ai_addr, server.ai_addrlen);
  }


//*****************************************************
// TcpSocket::Listen
//*****************************************************
int TcpSocket::Listen (int backlog) {
  printf ("%s\n", __func__);
  return listen(sockId, backlog);
}

//*****************************************************
// TcpSocket::Connect
//*****************************************************
int TcpSocket::Connect( uint16_t port) {
  printf ("%s\n", __func__);
  struct sockaddr_in server;

  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_family = AF_INET;
  server.sin_port = htons( port );


  if (connect(sockId, ( struct sockaddr *)&server , sizeof(server)) < 0) {
    printf ("Error Connecting %s\n", strerror(errno));
    return -1;
  }
  return 0;
}

//*****************************************************
// TcpSocket::Connect
//*****************************************************
int TcpSocket::Connect(std::string ipAddr, std::string port) {	
   printf ("%s\n", __func__);
   struct addrinfo *result;
   struct addrinfo server;
   
   memset(&server, 0, sizeof(struct addrinfo));
   server.ai_family = mFamily;    /* Allow IPv4 or IPv6 */
   server.ai_socktype = mType; 
   server.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
   server.ai_protocol = IPPROTO_TCP;    
   server.ai_canonname = NULL;
   server.ai_addr = NULL;
   server.ai_next = NULL;
  
   int status = getaddrinfo(ipAddr.c_str(), port.c_str(), &server, &result);
   if (status != 0) {
      printf("getaddrinfo: %s\n", gai_strerror(status));
      return 1;
   } else {
      struct addrinfo *rp;
      for (rp = result; rp != NULL; rp = rp->ai_next) {     
          printf("IP:%s %s %s %s\n", 
               GetIpString(rp->ai_addr).c_str(),
               Family(rp->ai_family).c_str(), 
               Type(rp->ai_socktype).c_str(), 
               Protocol(rp->ai_protocol).c_str()
               );
      }
   }

   server.ai_addrlen = result->ai_addrlen;
   server.ai_addr = result->ai_addr;

   freeaddrinfo(result);

  if (connect(sockId, server.ai_addr, server.ai_addrlen) < 0) {
    printf ("Connect failed: %s\n", strerror(errno));
    return -1;
  }
  return 0;
}


//*****************************************************
// TcpSocket::Accept
//*****************************************************
TcpSocket* TcpSocket::Accept (void) {
  struct sockaddr peer_addr;
  socklen_t peer_addr_size;

  printf ("%s \n", __func__);

  peer_addr_size = sizeof(struct sockaddr);
  int newSocket = accept(sockId, &peer_addr, &peer_addr_size);

  if (newSocket == -1)
  {
    printf ("accept error %s\n", strerror(errno));
    return nullptr;
  }

  TcpSocket *sock = new TcpSocket(*this);
  sock->AcceptedSocket(newSocket);

  printf ("Connected to %s\n", GetIpString(&peer_addr).c_str());

  return sock ;
 }



//*****************************************************
// TcpSocket::Send
//*****************************************************
int TcpSocket::Send(std::string msg) {
  uint8_t* pMsg = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(msg.c_str()));
  return Send(pMsg, msg.length());
}




//*****************************************************
// TcpSocket::AcceptedSocket
//*****************************************************
void TcpSocket::AcceptedSocket(int acceptedId) {
  sockId = acceptedId;
}