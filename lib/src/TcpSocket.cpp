#include <functional>
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
TcpSocket::TcpSocket() :  Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) {
  if (mDebug) printf ("%s\n", __func__);
  Socket::Open();
  peer_addr.sa_family = AF_UNSPEC;
}

//*****************************************************
// TcpSocket::TcpSocket
//*****************************************************
TcpSocket::TcpSocket (const TcpSocket& sock) :  Socket(sock) {
  peer_addr = sock.peer_addr;
}

//*****************************************************
// TcpSocket::~TcpSocket
//*****************************************************
TcpSocket::~TcpSocket() {
  if (mDebug) printf ("%s\n", __func__);
  Close();
}

//*****************************************************
// TcpSocket::Bind
//*****************************************************
int TcpSocket::Bind(std::string ipAddr, std::string port) {
   if (mDebug) printf ("%s\n", __func__);
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

   return bind(sockId, server.ai_addr, server.ai_addrlen);
  }


//*****************************************************
// TcpSocket::Listen
//*****************************************************
int TcpSocket::Listen (int backlog) {
  if (mDebug) printf ("%s\n", __func__);
  return listen(sockId, backlog);
}

//*****************************************************
// TcpSocket::Connect
//*****************************************************
int TcpSocket::Connect( uint16_t port) {
  if (mDebug) printf ("%s\n", __func__);
  struct sockaddr_in server;

  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_family = AF_INET;
  server.sin_port = htons( port );


  if (connect(sockId, ( struct sockaddr *)&server , sizeof(server)) < 0) {
    fprintf (stderr, "%s failed: %s\n", __func__, strerror(errno));
    return -1;
  }
  return 0;
}

//*****************************************************
// TcpSocket::Connect
//*****************************************************
int TcpSocket::Connect(std::string ipAddr, std::string port) {	
   if (mDebug) printf ("%s\n", __func__);
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
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
      return -1;
   } else {
      struct addrinfo *rp;
      for (rp = result; rp != NULL; rp = rp->ai_next) {     
          printf("CONNECT IP:%s %s %s %s\n", 
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

  if (connect(sockId, server.ai_addr, server.ai_addrlen) < 0) {
    fprintf (stderr, "%s failed: %s\n", __func__, strerror(errno));
    return -1;
  }
  return 0;
}


//*****************************************************
// TcpSocket::Accept
//*****************************************************
TcpSocket* TcpSocket::Accept (void) {
  socklen_t peer_addr_size;

  if (mDebug) printf ("%s \n", __func__);

  peer_addr_size = sizeof(struct sockaddr);
  int newSocket = accept(sockId, &peer_addr, &peer_addr_size);

  if (newSocket == -1)
  {
    fprintf (stderr, "accept error %s\n", strerror(errno));
    return nullptr;
  }

  TcpSocket *sock = new TcpSocket(*this);
  sock->AcceptedSocket(newSocket);

  printf ("Connected to %s\n", IpToString(&peer_addr).c_str());
  return sock ;
 }



//*****************************************************
// TcpSocket::GetPeerAddr
//*****************************************************
std::string TcpSocket::GetPeerAddr() {
  return IpToString(&peer_addr);
}






//*****************************************************
// TcpSocket::AcceptedSocket
//*****************************************************
void TcpSocket::AcceptedSocket(int acceptedId) {
  sockId = acceptedId;
}


//*****************************************************
// TcpSocket::Recv
//*****************************************************
int TcpSocket::Recv( char * buf, size_t len) { 
   return Recv( reinterpret_cast<uint8_t*>(buf), len);
}

//*****************************************************
// TcpSocket::Recv
//*****************************************************
int TcpSocket::Recv( uint8_t * buf, size_t len) { 
  int status = recv (sockId, buf, len, 0);
   if (status < 0) {
        fprintf(stderr,"%s error: %s\n", __func__,strerror(errno));
    }
   return (status);
}

//*****************************************************
// TcpSocket::Recv
//*****************************************************
int TcpSocket::Recv( char * buf, size_t len, timeval &t) { 
   return Recv( reinterpret_cast<uint8_t*>(buf), len, t);
}

//*****************************************************
// TcpSocket::Recv
//*****************************************************
int TcpSocket::Recv( uint8_t * buf, size_t len, timeval &t) { 
  std::vector<Socket> readSockList;
  Socket& s = std::ref(*static_cast<Socket*>(this));
  readSockList.push_back(s);
  int status = Socket::Select(&readSockList, nullptr, nullptr, t);
  if (status > 0) {
    status = recv (sockId, buf, len, 0);
     if (status < 0) {
          fprintf(stderr,"%s error: %s\n", __func__,strerror(errno));
      }
  }
   return (status);
}




//*****************************************************
// TcpSocket::Send
//*****************************************************
int TcpSocket::Send( const char * buf, size_t len) { 
  return Send(reinterpret_cast<const uint8_t*>(buf), len);
}

//*****************************************************
// TcpSocket::Send
//*****************************************************
int TcpSocket::Send( const uint8_t * buf, size_t len) { 
  int status = send (sockId, buf, len, 0);
   if (status < 0) {
        fprintf(stderr,"%s error: %s\n", __func__,strerror(errno));
    }
   return (status);
}

//*****************************************************
// TcpSocket::Send
//*****************************************************
int TcpSocket::Send(const std::string msg) {
  uint8_t* pMsg = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(msg.c_str()));
  return Send(pMsg, msg.length());
}