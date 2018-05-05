#include "../inc/Cube_SocketTCP.h"


BOOL Cube_SocketTCP_Server::Initialize( unsigned int Port )
{
	WORD wVersionRequested; 
	WSADATA wsaData;        
	int err;                

	wVersionRequested = MAKEWORD( 1, 1 );


	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {

		return FALSE;
	}

	if ( LOBYTE( wsaData.wVersion ) != 1 ||
		HIBYTE( wsaData.wVersion ) != 1 ) {
			WSACleanup();
			return FALSE;
	}
	//Initialize socket
	BOOL optval=TRUE;
	SOCKADDR_IN sockaddr_in;

	if ((m_Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==INVALID_SOCKET)
	{
		return FALSE;
	}


	sockaddr_in.sin_family=AF_INET;
	sockaddr_in.sin_addr.s_addr=INADDR_ANY;
	sockaddr_in.sin_port=htons(Port);

	if (bind(m_Socket,(LPSOCKADDR)&sockaddr_in,sizeof(sockaddr_in))==SOCKET_ERROR)
	{
		closesocket(m_Socket);
		return FALSE;
	}
	
	if (listen(m_Socket,5)==SOCKET_ERROR)
	{
		closesocket(m_Socket);
		return FALSE;
	}

	run();

	return TRUE;
}

BOOL Cube_SocketTCP_Server::Initialize( Cube_SocketTCP_Server_IO _IO)
{
	return Initialize(_IO.Port);
}



Cube_TCPConnector Cube_SocketTCP_Server::Accept()
{
	int length = sizeof(sockaddr_in); 
	Cube_TCPConnector client;
	while((client.s=accept(m_Socket, (struct sockaddr *)&client.in, &length))==SOCKET_ERROR);
	return client;
}




void Cube_SocketTCP_Server::free()
{
	Close();
	terminate();
}

void Cube_SocketTCP_Server::Close()
{
	if (m_Socket!=0)
	{
		closesocket(m_Socket);
	}
	m_Socket=0;
}

void Cube_SocketTCP_Server::run()
{
	while (TRUE)
	{
		OnConnect(Accept());
	}

}





BOOL Cube_SocketTCP_Client::Initialize()
{

	WORD wVersionRequested; 
	WSADATA wsaData;        
	int err;                

	wVersionRequested = MAKEWORD( 1, 1 );


	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {

		return FALSE;
	}

	if ( LOBYTE( wsaData.wVersion ) != 1 ||
		HIBYTE( wsaData.wVersion ) != 1 ) {
			WSACleanup();
			return FALSE;
	}
	//Initialize socket


	if ((m_Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==INVALID_SOCKET)
	{
		return FALSE;
	}


	return TRUE;
}

BOOL Cube_SocketTCP_Client::Connect( const char *IP,unsigned int Port )
{
	SOCKADDR_IN sockaddr_in;
	sockaddr_in.sin_family=AF_INET;
	sockaddr_in.sin_addr.s_addr=inet_addr(IP);
	sockaddr_in.sin_port=htons(Port);

	int ret = connect(m_Socket, (struct sockaddr *)&sockaddr_in, sizeof(SOCKADDR_IN));  
	if (ret == SOCKET_ERROR)  
	{  
		closesocket(m_Socket);   
		WSACleanup();  
		return FALSE;
	}  
	return TRUE;
}

size_t Cube_SocketTCP_Client::ReceiveData(void *Buffer,size_t size )
{
	if (!m_Socket)
	{
		return 0;
	}
	int ssize;
	if (ssize=recv(m_Socket,(char *)Buffer,size,0)==SOCKET_ERROR)
	{
		return 0;
	}
	return ssize;
}

BOOL Cube_SocketTCP_Client::SendData( void *Buffer,size_t size )
{
	int length;         
	if (m_Socket==0)
	{
		return FALSE;
	}

	while(size>0)
	{
		if ((length=send(m_Socket,(const char *)Buffer,size,0))==SOCKET_ERROR)
		{
			return FALSE;
		}
		size-=length;
	}
	return TRUE;
}

void Cube_SocketTCP_Client::free()
{
	Close();
}

void Cube_SocketTCP_Client::Close()
{
	if(m_Socket){
		closesocket(m_Socket);
		m_Socket=0;
	}
}

BOOL Cube_SocketTCP_Connect::Initialize( Cube_SocketTCP_Connect_IO &info )
{
	m_Client=info.client;
	run();
	return TRUE;
}

void Cube_SocketTCP_Connect::run()
{
	int ssize;
	Cube_SocketTCP_Connect_I _in;
	memset(&_in,0,sizeof(_in));
	while(TRUE)
	{
		if ((ssize=recv(m_Client.s,(char *)_in.Buffer,CUBE_SOCKETTCP_I_BUFFERSIZE,0))==SOCKET_ERROR)
		{
		break;
		}
		if (ssize!=0)
		{
			Recv(_in);
		}
		else
		{
			break;
		}
	}
	Offline();
	Close();
}

void Cube_SocketTCP_Connect::Close()
{
	closesocket(m_Client.s);
}

char* Cube_SocketTCP_Connect::GetIP()
{
	return inet_ntoa(m_Client.in.sin_addr);
}

size_t Cube_SocketTCP_Connect::Send( Cube_SocketTCP_Connect_O &Out )
{
	int length;         
	if (m_Client.s==0)
	{
		return FALSE;
	}

	if ((length=send(m_Client.s,(const char *)Out.Buffer,Out.Size,0))==SOCKET_ERROR)
	{
		return 0;
	}

	return length;
}
