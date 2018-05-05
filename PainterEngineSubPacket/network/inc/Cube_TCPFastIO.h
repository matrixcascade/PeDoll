#pragma once

//#include "Cube_SocketTCP.h"
#include <winsock2.h>  
#include <ws2tcpip.h>  
#include <mswsock.h> 
#include "../../../typedef/typedef.h"
#include "Cube_Thread.h"

#define CUBE_FASTIO_BUFFER_LEN				  8*1024	//8k buffer for IOCP
#define CUBE_FASTIO_THREAD_PROCESSER_PERCOUNT 2			//thread count per core 
#define CUBE_FASTIO_EXIT_CODE				  0xFF		//Exit code for parameter
enum _CUBE_TCPFASTIO_OPERATION_TYPE
{
	ACCEPT_POSTED,                     
	SEND_POSTED,                       
	RECV_POSTED,                       
	NULL_POSTED                        
};

struct CUBE_WSABUF {
	ULONG len;     /* the length of the buffer */
	CHAR FAR *buf; /* the pointer to the buffer */
};



struct IO_CONTEXT
{
	OVERLAPPED						m_Overlapped;           
	SOCKET							m_socket;
	SOCKADDR_IN						m_addrin;
	_CUBE_TCPFASTIO_OPERATION_TYPE	m_OpType;              
	pt_int						    m_Flag;
	WSABUF							m_wsaBuf;
	DWORD							m_Size;
	pt_byte							m_IOBuffer[CUBE_FASTIO_BUFFER_LEN];
	IO_CONTEXT						*m_pNextContext;
	IO_CONTEXT						*m_pPreviousContext;
	IO_CONTEXT()
	{
		Reset();
	}
	void ResetBuffer()
	{
		ZeroMemory(&m_Overlapped, sizeof(m_Overlapped));
		ZeroMemory(&m_Overlapped, sizeof(m_Overlapped));
	}
	void Reset()
	{
		m_wsaBuf.buf = m_IOBuffer;
		m_wsaBuf.len = CUBE_FASTIO_BUFFER_LEN;
		m_pNextContext = NULL;
		m_pPreviousContext = NULL;
		ZeroMemory(&m_Overlapped, sizeof(m_Overlapped));
		ZeroMemory(&m_addrin, sizeof(m_addrin));
		ZeroMemory(&m_IOBuffer, sizeof(m_IOBuffer));
		m_socket = INVALID_SOCKET;
		m_OpType = NULL_POSTED;
		m_Flag = 0;
	}
};


class Cube_TCPFastIO_Server;


class Cube_TCPFastIO_ServerProcessor:public Cube_Thread
{
public:
	Cube_TCPFastIO_ServerProcessor()
	{
		m_pcCube_TCPFastIO_Server = NULL;
	}
	Cube_TCPFastIO_ServerProcessor(Cube_TCPFastIO_Server *pServer)
	{
		m_pcCube_TCPFastIO_Server = pServer;
	}
	void SetServerPointer(Cube_TCPFastIO_Server *pserver)
	{
		m_pcCube_TCPFastIO_Server = pserver;
	}
	void run();
private:
	Cube_TCPFastIO_Server *m_pcCube_TCPFastIO_Server;
};


class Cube_TCPFastIO_Server
{
public:
	Cube_TCPFastIO_Server()
	{
		InitializeCriticalSection(&m_cs);
		m_ServerProcessors = NULL;
		m_MaxConnection = 0xffffffff;
		m_CurrentConnection = 0;

		m_ContextListLast=NULL;
		m_ThreadCount=0;
		m_Socket=INVALID_SOCKET;
		m_hIOCompletionPort=INVALID_HANDLE_VALUE;
		m_ShutdownEvent=INVALID_HANDLE_VALUE;
		m_ServerProcessors=NULL;
		m_MaxConnection=0;
		m_CurrentConnection=0;

	};
	~Cube_TCPFastIO_Server() 
	{

	};

	bool Start(int Port,int ThreadProcessCore=0,unsigned int MaxConnection=0xffffffff);
	void Stop();

	void * AcceptExInterface();
	void * GetAcceptSockAddrsInterface();


	HANDLE		 GetIOCompletedPort();
	HANDLE		 GetShutdownEvent();
	SOCKET		 GetSocket();
	
	void		 opEnterCriticalSection();
	void		 opLeaveCriticalsection();
	bool		 Send(IN SOCKET socket, pt_byte *Buffer, pt_ulong Size);
	bool		 _PostAccept();
	bool		 _PostRecv(IO_CONTEXT *context);

	IO_CONTEXT   *mallocNewContext();
	void		 RemoveContext(IO_CONTEXT *context);
	void		 DisconnectEvent(IN pt_int flag);
	void		 ConnectEvent(IN SOCKET socket, IN SOCKADDR_IN	addrin, OUT pt_int &Flag);

	virtual	void OnConnected(IN SOCKET socket,IN SOCKADDR_IN	addrin,OUT pt_int &Flag) = 0;
	virtual void OnReceived(IN pt_int flag, IN pt_byte *Buffer, IN int Size) = 0;
	virtual void OnDisconnected(IN pt_int flag) = 0;
	virtual void OnError() {};
private:
	bool		InitializeListenSocket(int Port);
	IO_CONTEXT					*m_ContextListLast;
	CubeCriticalSection			m_cs;
	int							m_ThreadCount;
	SOCKET						m_Socket;
	HANDLE						m_hIOCompletionPort;
	HANDLE						m_ShutdownEvent;
	LPFN_ACCEPTEX               m_lpfnAcceptEx;                
	LPFN_GETACCEPTEXSOCKADDRS   m_lpfnGetAcceptExSockAddrs;
	Cube_TCPFastIO_ServerProcessor *m_ServerProcessors;
	unsigned int				m_MaxConnection;
	unsigned int				m_CurrentConnection;
};

class Cube_TCPFastIO_Client;

class Cube_TCPFastIO_ClientProcessor :public Cube_Thread
{
public:
	Cube_TCPFastIO_ClientProcessor()
	{
		m_pcCube_TCPFastIO_Client = NULL;
	};
	Cube_TCPFastIO_ClientProcessor(Cube_TCPFastIO_Client *pClient)
	{
		m_pcCube_TCPFastIO_Client = pClient;
	};
	void SetServerPointer(Cube_TCPFastIO_Client *pClient)
	{
		m_pcCube_TCPFastIO_Client = pClient;
	};
	void run();

private:
	Cube_TCPFastIO_Client *m_pcCube_TCPFastIO_Client;
};

class Cube_TCPFastIO_Client
{
public:
	Cube_TCPFastIO_Client()	
	{
		m_IsConnecting=false;
		m_ContextListLast=NULL;
		m_Socket=INVALID_SOCKET;
		m_ThreadCount=0;
		m_ShutdownEvent=INVALID_HANDLE_VALUE;
		m_ClientProcessors=NULL;
	};
	~Cube_TCPFastIO_Client() {};
	void		InitializeNetwork();
	void		FreeNetwork();

	bool		 Connect(const char *addr,int port,int ProcessorsCount=1);
	bool		 Connect(pt_ulong ulIP,int port,int ProcessorsCount=1);
	bool		 Connect(SOCKADDR_IN addrin,int ProcessorsCount=1);
	void		 Disconnect();

	void		 FreeSocket();
	BOOL		 IsConnecting();
	HANDLE		 GetIOCompletedPort();
	HANDLE		 GetShutdownEvent();
	SOCKET		 GetSocket();

	void		 OnDisconnectEvent(int flag);

	bool		 Send(pt_byte *Buffer, pt_ulong Size);
	bool		 _PostRecv(IO_CONTEXT *context);

	virtual void OnReceived(IN pt_int flag, IN pt_byte *Buffer, IN int Size) = 0;
	virtual void OnDisconnected(IN pt_int flag) = 0;
	virtual void OnError() {};
	bool		 m_IsConnecting;

private:
	bool Start(int ProcessorsCount=1);
	void Stop();

	IO_CONTEXT   *mallocNewContext();
	void		 RemoveContext(IO_CONTEXT *context);

	IO_CONTEXT					*m_ContextListLast;
	int							m_ThreadCount;
	SOCKET						m_Socket;
	HANDLE						m_hIOCompletionPort;
	HANDLE						m_ShutdownEvent;
	Cube_TCPFastIO_ClientProcessor *m_ClientProcessors;
};

