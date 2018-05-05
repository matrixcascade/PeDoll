#pragma once

#include <vector>

#include "Cube_SocketHeader.h"
#include "Cube_AsynIO.h"


#define  CUBE_SOCKETTCP_I_BUFFERSIZE   4096//4k for Buffer

struct Cube_TCPConnector
{
	SOCKET       s;
	SOCKADDR_IN  in;
	Cube_TCPConnector()
	{
		s=SOCKET_ERROR;
		memset(&in,0,sizeof SOCKADDR_IN);
	}
};




struct Cube_SocketTCP_Server_IO
{
	unsigned int Port;
};


struct Cube_SocketTCP_Connect_IO
{
	Cube_TCPConnector client;
};


struct Cube_SocketTCP_Connect_I
{
	unsigned char Buffer[CUBE_SOCKETTCP_I_BUFFERSIZE];
	size_t Size;

	Cube_SocketTCP_Connect_I()
	{
		Size=0;
	}
};

struct Cube_SocketTCP_Connect_O
{
	void *Buffer;
	size_t Size;

	Cube_SocketTCP_Connect_O()
	{
		Buffer=NULL;
		Size=0;
	}
};


using namespace std;


class Cube_SocketTCP_Connect :
	public Cube_AsynIO<Cube_SocketTCP_Connect_IO,Cube_SocketTCP_Connect_I,Cube_SocketTCP_Connect_O>
{
public:
	Cube_SocketTCP_Connect(){};
	~Cube_SocketTCP_Connect(){};

	BOOL			Initialize(Cube_SocketTCP_Connect_IO &info) override;
	void			run()										override;			
	size_t			Send(Cube_SocketTCP_Connect_O &Out)			override;
	virtual void    Recv(Cube_SocketTCP_Connect_I &in)          =0;
	virtual void    Offline(){};
	void			Close()										override;
	char*			GetIP();
private:
	Cube_TCPConnector m_Client;
};




class Cube_SocketTCP_Server:public Cube_Thread
{
public:
	Cube_SocketTCP_Server(void){};
	~Cube_SocketTCP_Server(void){};

	BOOL				Initialize(Cube_SocketTCP_Server_IO);							
	BOOL				Initialize(unsigned int Port);
	Cube_TCPConnector   Accept();

	void				run()override;
	void				free();
	void				Close();
	
	virtual void		OnConnect(Cube_TCPConnector)				=0;
private:
	SOCKET					m_Socket;  
	
};

class Cube_SocketTCP_Client
{
public:
	Cube_SocketTCP_Client(void) {};
	~Cube_SocketTCP_Client(void) {};

	BOOL			Initialize();
	BOOL         	Connect(const char *IP,unsigned int Port);
	size_t			ReceiveData(void *Buffer,size_t size);
	BOOL			SendData(void *Buffer,size_t size);

	void			free();
	void			Close();
private:
	SOCKET			m_Socket;  

};