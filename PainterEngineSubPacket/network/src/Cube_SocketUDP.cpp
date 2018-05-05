#include "../inc/Cube_SocketUDP.h"


Cube_SocketUDP::Cube_SocketUDP(void)
{
}


Cube_SocketUDP::~Cube_SocketUDP(void)
{
}

BOOL Cube_SocketUDP::SocketInitialize( unsigned int Port )
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

	if ((m_Socket=socket(AF_INET,SOCK_DGRAM,0))==INVALID_SOCKET)
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
	if (setsockopt(m_Socket,SOL_SOCKET,SO_BROADCAST,(char *)&optval,sizeof(sockaddr_in))==SOCKET_ERROR)
	{
		closesocket(m_Socket);
		return FALSE;
	}
	return TRUE;
}

BOOL Cube_SocketUDP::Initialize( Cube_SocketUDP_IO &IO)
{
	return SocketInitialize(IO.Port);
}



BOOL Cube_SocketUDP::SendData( void *pSendDataBuffer,size_t Size,SOCKADDR_IN to )
{
	int length;
	while(Size>0)
	{
		if ((length=sendto(m_Socket,(const char *)pSendDataBuffer,Size,0,(LPSOCKADDR)&to,sizeof(SOCKADDR)))==SOCKET_ERROR)
		{
			int i=GetLastError(); 
			return FALSE;
		}
		Size-=length;
	}
	return TRUE;
}



size_t Cube_SocketUDP::ReceiveData( void *Buffer,size_t size,SOCKADDR_IN &In )
{
	assert(m_Socket!=0);
	int SockAddrSize=sizeof(SOCKADDR);
	size_t ReturnSize;
	if((ReturnSize=recvfrom(m_Socket,(char *)Buffer,size,0,(LPSOCKADDR)&In,&SockAddrSize))!=SOCKET_ERROR)
		return ReturnSize;
	else
		return 0;
}

void Cube_SocketUDP::free()
{
	if (m_Socket!=0)
	{
		closesocket(m_Socket);
	}

	m_Socket=0;
}

void Cube_SocketUDP::Close()
{
	free();
}

char* Cube_SocketUDP::GetIP( SOCKADDR_IN &sockaddr_in )
{
	return inet_ntoa(sockaddr_in.sin_addr);
}

size_t Cube_SocketUDP::Send( Cube_SocketUDP_O &Out )
{
	return SendData(Out.Buffer,Out.Size,Out.to);
}

void Cube_SocketUDP::run()
{
	static Cube_SocketUDP_I _in;
	while (TRUE)
	{
		_in.Size=ReceiveData(_in.Buffer,CUBE_SOCKETUDP_I_BUFFERSIZE,_in.in);

		if(_in.Size)
		Recv(_in);
	}
}

void Cube_SocketUDP::Recv( Cube_SocketUDP_I &in )
{

}

