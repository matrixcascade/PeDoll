#pragma once
#include "Cube_SocketHeader.h"
#include "Cube_AsynIO.h"

#define  CUBE_SOCKETUDP_I_BUFFERSIZE   4096//4k for Buffer

struct Cube_SocketUDP_IO
{
	unsigned int Port;
};

struct Cube_SocketUDP_I
{
	unsigned char Buffer[CUBE_SOCKETUDP_I_BUFFERSIZE];
	size_t Size;
	SOCKADDR_IN in;
	Cube_SocketUDP_I()
	{
		Size=0;
	}

	Cube_SocketUDP_I(void *Buffer,size_t Size,SOCKADDR_IN in)
	{
		this->Size=Size;
		this->in=in;
	}

};

struct Cube_SocketUDP_O
{
	Cube_SocketUDP_O()
	{

	}
	Cube_SocketUDP_O(void *Buffer,size_t Size,char *pDestIPaddr,int Port)
	{
		this->Buffer=Buffer;
		this->Size=Size;

		to.sin_family=AF_INET;
		if (pDestIPaddr!=NULL)
		{
			if (INADDR_NONE == inet_addr(pDestIPaddr))
				goto _ERROR;
			to.sin_addr.s_addr=inet_addr(pDestIPaddr);
		}
		else
		{
			to.sin_addr.s_addr=INADDR_BROADCAST;
		}

		to.sin_port=htons(Port);

		_ERROR:
		NULL;
	}

	Cube_SocketUDP_O(void *Buffer,size_t Size,SOCKADDR_IN to)
	{
		this->Buffer=Buffer;
		this->Size=Size;
		this->to=to;

	}
	Cube_SocketUDP_O(void *Buffer,size_t Size,unsigned short Port,unsigned long IP)
	{
		this->Buffer=Buffer;
		this->Size=Size;

		to.sin_family=AF_INET;
		to.sin_addr.s_addr=IP;
		to.sin_port=htons(Port);

	}

	SOCKADDR_IN to;
	void *Buffer;
	size_t Size;
};


class Cube_SocketUDP:public Cube_AsynIO<Cube_SocketUDP_IO,Cube_SocketUDP_I,Cube_SocketUDP_O>
{
public:
	Cube_SocketUDP(void);
	~Cube_SocketUDP(void);


	BOOL			Initialize(Cube_SocketUDP_IO&) ;
	virtual size_t	Send(Cube_SocketUDP_O &Out) override;
	virtual void	Recv(Cube_SocketUDP_I &in)	override;
	void			run()						override;
	


	BOOL			SocketInitialize(unsigned int Port);
	BOOL			SendData(void *pSendDataBuffer,size_t Size,SOCKADDR_IN to);
	size_t			ReceiveData(void *Buffer,size_t size,SOCKADDR_IN &In);

	void			free();
	void			Close();

	char*			GetIP(SOCKADDR_IN &sockaddr_in);
private:
	SOCKET			m_Socket;      
};

