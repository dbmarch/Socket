#include <stdio.h>
#include <errno.h>
#include <string.h>
#include<sys/socket.h> 
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include "TcpSocket.h"

//*****************************************************
// TcpSocket::TcpSocket
//*****************************************************
TcpSocket::TcpSocket() :
  Socket(AF_INET, SOCK_STREAM)
{
  printf ("%s\n", __func__);
  Socket::Open();
}

//*****************************************************
// TcpSocket::TcpSocket
//*****************************************************
TcpSocket::TcpSocket (const TcpSocket& sock) :
  Socket(sock)
{
}

//*****************************************************
// TcpSocket::~TcpSocket
//*****************************************************
TcpSocket::~TcpSocket()
{
  printf ("%s\n", __func__);
}



//*****************************************************
// TcpSocket::Bind
//*****************************************************
int TcpSocket::Bind(in_addr_t addr, uint16_t port)
{
  printf ("%s\n", __func__);

  struct sockaddr_in server;
  
  //Prepare the sockaddr_in structure
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = addr;
  server.sin_port = htons( port);
  return bind(sockId, (const struct sockaddr*)  &server, sizeof(server));
}


//*****************************************************
// TcpSocket::Listen
//*****************************************************
int TcpSocket::Listen (int backlog) 
{
  printf ("%s\n", __func__);
  return listen(sockId, backlog);
}

//*****************************************************
// TcpSocket::Connect
//*****************************************************
int TcpSocket::Connect( uint16_t port) 
{
  printf ("%s\n", __func__);
  struct sockaddr_in server;

  server.sin_addr.s_addr = INADDR_ANY;
	server.sin_family = AF_INET;
	server.sin_port = htons( port );

  if (connect(sockId, ( struct sockaddr *)&server , sizeof(server)) < 0)
  {
    printf ("Error Connecting %s\n", strerror(errno));
    return -1;
  }
  return 0;

}

//*****************************************************
// TcpSocket::Connect
//*****************************************************
int TcpSocket::Connect(const char* ipAddr, uint16_t port)
{	
  printf ("%s\n", __func__);
  struct sockaddr_in server;

  server.sin_addr.s_addr = inet_addr(ipAddr);
	server.sin_family = AF_INET;
	server.sin_port = htons( port );

  if (connect(sockId, ( struct sockaddr *)&server , sizeof(server)) < 0)
  {
    printf ("Error Connecting %s\n", strerror(errno));
    return -1;
  }
  return 0;
}


//*****************************************************
// TcpSocket::Accept
//*****************************************************
TcpSocket* TcpSocket::Accept (void)
{
  struct sockaddr peer_addr;
  socklen_t peer_addr_size;

  printf ("%s \n", __func__);

  peer_addr_size = sizeof(struct sockaddr);
  int newSocket = accept(sockId, (struct sockaddr*) &peer_addr, &peer_addr_size);

  if (newSocket == -1)
  {
    printf ("accept error %s\n", strerror(errno));
    return nullptr;
  }

  TcpSocket *sock = new TcpSocket(*this);
  sock->AcceptedSocket(newSocket);

  return sock ;
 }



//*****************************************************
// TcpSocket::Send
//*****************************************************
int TcpSocket::Send(std::string msg)
{
  uint8_t* pMsg = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(msg.c_str()));
  return Send(pMsg, msg.length());
}




//*****************************************************
// TcpSocket::AcceptedSocket
//*****************************************************
void TcpSocket::AcceptedSocket(int acceptedId)
{
  sockId = acceptedId;
}


