#include "../inc/Cube_TCPFastIO.h"
#include <stdio.h>

void Cube_TCPFastIO_ServerProcessor::run()
{

	OVERLAPPED						*pOverlapped = NULL;
	DWORD							dwBytesTransfered = 0;
	pt_int							ThreadOpCode;
	while (WAIT_OBJECT_0 != WaitForSingleObject(m_pcCube_TCPFastIO_Server->GetShutdownEvent(), 0))
	{
		BOOL bReturn = GetQueuedCompletionStatus(
			m_pcCube_TCPFastIO_Server->GetIOCompletedPort(),
			&dwBytesTransfered,
			(PULONG_PTR)&ThreadOpCode,
			&pOverlapped,
			INFINITE);
		if (ThreadOpCode==CUBE_FASTIO_EXIT_CODE)
		{
			break;
		}
		IO_CONTEXT* pIoContext = CONTAINING_RECORD(pOverlapped, IO_CONTEXT, m_Overlapped);

		if (!bReturn)
		{
			DWORD dwErr = GetLastError();
			
			if (ERROR_NETNAME_DELETED == dwErr)
			{
				m_pcCube_TCPFastIO_Server->DisconnectEvent(pIoContext->m_Flag);
				continue;
			}
			else
			{
				goto ERR;
			}
			continue;
		}
		else
		{
			
			if ((0 == dwBytesTransfered) && (RECV_POSTED == pIoContext->m_OpType || SEND_POSTED == pIoContext->m_OpType))
			{
				m_pcCube_TCPFastIO_Server->DisconnectEvent(pIoContext->m_Flag);
				continue;
			}
			else
			{
				switch (pIoContext->m_OpType)
				{
				case ACCEPT_POSTED:
				{
					SOCKADDR_IN* ClientAddr = NULL;
					SOCKADDR_IN* LocalAddr = NULL;
					pt_int		 __flag;
					int remoteLen = sizeof(SOCKADDR_IN), localLen = sizeof(SOCKADDR_IN);
					((LPFN_GETACCEPTEXSOCKADDRS)(m_pcCube_TCPFastIO_Server->GetAcceptSockAddrsInterface()))(pIoContext->m_wsaBuf.buf, /*CUBE_FASTIO_BUFFER_LEN - ((sizeof(SOCKADDR_IN) + 16) * 2)*/0,
						sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, (LPSOCKADDR*)&LocalAddr, &localLen, (LPSOCKADDR*)&ClientAddr, &remoteLen);


					m_pcCube_TCPFastIO_Server->ConnectEvent(pIoContext->m_socket,*ClientAddr,__flag);

					HANDLE hTemp = CreateIoCompletionPort((HANDLE)pIoContext->m_socket, m_pcCube_TCPFastIO_Server->GetIOCompletedPort(),0, 0);

					if (NULL == hTemp)
					{
						goto ERR;
					}

					IO_CONTEXT *pNewContext = m_pcCube_TCPFastIO_Server->mallocNewContext();
					if (pNewContext==NULL)
					{
						goto ERR;
					}

					pNewContext->m_addrin = *ClientAddr;
					pNewContext->m_Flag = __flag;
					pNewContext->m_socket = pIoContext->m_socket;

					if (false == m_pcCube_TCPFastIO_Server->_PostRecv(pNewContext))
					{
						goto ERR;
					}


					m_pcCube_TCPFastIO_Server->RemoveContext(pIoContext);

				}
				break;

				// RECV
				case RECV_POSTED:
				{
					m_pcCube_TCPFastIO_Server->OnReceived(pIoContext->m_Flag, pIoContext->m_IOBuffer, dwBytesTransfered);
					m_pcCube_TCPFastIO_Server->_PostRecv(pIoContext);
				}
				break;
				case SEND_POSTED:
				{
					//nop
				}
				break;
				default:
					break;
				} //switch
			}//if
		}//if

	}//while

	return;
ERR:
	m_pcCube_TCPFastIO_Server->OnError();
	return;
}




bool Cube_TCPFastIO_Server::Start(int Port, int ThreadProcessCore,unsigned int MaxConnection)
{
	
	
	m_ShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_ShutdownEvent==INVALID_HANDLE_VALUE)
	{
		return false;
	}

	m_hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if (NULL == m_hIOCompletionPort)
	{
		goto _ERR;
	}

	SYSTEM_INFO si;

	GetSystemInfo(&si);

	m_ThreadCount = CUBE_FASTIO_THREAD_PROCESSER_PERCOUNT * si.dwNumberOfProcessors;
	
	if (!InitializeListenSocket(Port))
		goto _ERR;
	
	//Create threads
	if (ThreadProcessCore!=0)
	{
		m_ThreadCount = ThreadProcessCore;
	}

	m_ServerProcessors = new Cube_TCPFastIO_ServerProcessor[m_ThreadCount];

	if (m_ServerProcessors==NULL)
	{
		goto _ERR;
	}
	for (int i=0;i<m_ThreadCount;i++)
	{
		m_ServerProcessors[i].SetServerPointer(this);
	}
	for (int i = 0; i < m_ThreadCount; i++)
	{
		m_ServerProcessors[i].start();
	}


	//Post first accept message
	m_MaxConnection = MaxConnection;
	for (int i = 0; i < m_ThreadCount; i++)
	{
		if(m_CurrentConnection<m_MaxConnection)
		_PostAccept();
		else
		break;
	}

	return true;

_ERR:
	Stop();
	return false;
}

void Cube_TCPFastIO_Server::Stop()
{
	SetEvent(m_ShutdownEvent);
	for (int i=0;i<m_ThreadCount;i++)
	{
		PostQueuedCompletionStatus(m_hIOCompletionPort, 0,CUBE_FASTIO_EXIT_CODE, NULL);
	}

	for (int i = 0; i < m_ThreadCount; i++)
	{
		m_ServerProcessors[i].WaitForExit();
	}
	
	while (m_ContextListLast!=NULL)
	{
		IO_CONTEXT *tempPointer = m_ContextListLast->m_pPreviousContext;
		if (m_ContextListLast->m_socket != INVALID_SOCKET)
			closesocket(m_ContextListLast->m_socket);
		delete m_ContextListLast;
		m_ContextListLast = tempPointer;
	}

	if(m_ServerProcessors)
	{
		if(m_ThreadCount==1)
		delete m_ServerProcessors;
		else
		delete[] m_ServerProcessors;
	}
	m_ServerProcessors = NULL;

	CloseHandle(m_ShutdownEvent);
	CloseHandle(m_hIOCompletionPort);

	m_ContextListLast=NULL;
	m_ThreadCount=0;
	m_Socket=INVALID_SOCKET;
	m_hIOCompletionPort=INVALID_HANDLE_VALUE;
	m_ShutdownEvent=INVALID_HANDLE_VALUE;
	m_ServerProcessors=NULL;
	m_MaxConnection=0;
	m_CurrentConnection=0;

	WSACleanup();
}


void * Cube_TCPFastIO_Server::AcceptExInterface()
{
	return m_lpfnAcceptEx;
}

void * Cube_TCPFastIO_Server::GetAcceptSockAddrsInterface()
{
	return m_lpfnGetAcceptExSockAddrs;
}

HANDLE Cube_TCPFastIO_Server::GetIOCompletedPort()
{
	return m_hIOCompletionPort;
}

HANDLE Cube_TCPFastIO_Server::GetShutdownEvent()
{
	return m_ShutdownEvent;
}

SOCKET Cube_TCPFastIO_Server::GetSocket()
{
	return m_Socket;
}

void Cube_TCPFastIO_Server::opEnterCriticalSection()
{
	CubeEnterCriticalSection(&m_cs);
}

void Cube_TCPFastIO_Server::opLeaveCriticalsection()
{
	CubeLeaveCriticalSection(&m_cs);
}

bool Cube_TCPFastIO_Server::Send(IN SOCKET socket, pt_byte *Buffer, pt_ulong Size)
{
	if (send(socket, Buffer, Size, 0) == SOCKET_ERROR) return false;
	return true;
}

bool Cube_TCPFastIO_Server::_PostAccept()
{
	IO_CONTEXT *pNewContext = mallocNewContext();
	pNewContext->Reset();
	pNewContext->m_OpType = ACCEPT_POSTED;
	pNewContext->m_Flag = 0;

	DWORD dwBytes = 0;

	pNewContext->m_socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == pNewContext->m_socket)
	{
		return false;
	}

	if (FALSE == m_lpfnAcceptEx(m_Socket, pNewContext->m_socket, pNewContext->m_wsaBuf.buf, /*CUBE_FASTIO_BUFFER_LEN-((sizeof(SOCKADDR_IN) + 16) * 2)*/0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes, &pNewContext->m_Overlapped))
	{
		if (WSA_IO_PENDING != WSAGetLastError())
		{
			return false;
		}
	}

	return true;
}

bool Cube_TCPFastIO_Server::_PostRecv(IO_CONTEXT *context)
{
	DWORD dwFlags = 0;
	DWORD dwBytes = 0;

	
	WSABUF *p_wbuf = &context->m_wsaBuf;
	OVERLAPPED *p_ol =&context->m_Overlapped;
	context->m_OpType = RECV_POSTED;


	int nBytesRecv = WSARecv(context->m_socket, p_wbuf, 1, &dwBytes, &dwFlags, p_ol, NULL);

	if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
	{
		return false;
	}
	return true;
}

IO_CONTEXT * Cube_TCPFastIO_Server::mallocNewContext()
{
	IO_CONTEXT *NewContext = new IO_CONTEXT;
	if (NewContext==NULL)
	{
		return NULL;
	}
	if (m_ContextListLast==NULL)
	{
		NewContext->Reset();
		m_ContextListLast = NewContext;
	}
	else
	{
		NewContext->Reset();
		NewContext->m_pPreviousContext = m_ContextListLast;
		m_ContextListLast = NewContext;
	}
	return NewContext;
}

void Cube_TCPFastIO_Server::RemoveContext(IO_CONTEXT *context)
{
	if (context->m_pPreviousContext!=NULL)
	{
		context->m_pPreviousContext->m_pNextContext = context->m_pNextContext;
		if (context->m_pNextContext!=NULL)
		{
			context->m_pNextContext->m_pPreviousContext = context->m_pPreviousContext;
		}
	}
	else
	{
		m_ContextListLast = context->m_pNextContext;
	}
	delete context;
}

void Cube_TCPFastIO_Server::DisconnectEvent(IN pt_int flag)
{
	m_CurrentConnection--;

	OnDisconnected(flag);
	_PostAccept();
}

void Cube_TCPFastIO_Server::ConnectEvent(IN SOCKET socket, IN SOCKADDR_IN addrin, OUT pt_int &Flag)
{
	m_CurrentConnection++;
	OnConnected(socket, addrin, Flag);
	if (m_CurrentConnection<m_MaxConnection)
	{
		_PostAccept();
	}
}

bool Cube_TCPFastIO_Server::InitializeListenSocket(int Port)
{
	GUID GuidAcceptEx = WSAID_ACCEPTEX;
	GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;

	struct sockaddr_in ServerAddress;

	WSADATA wsaData;

	int nResult;
	nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (NO_ERROR != nResult)
	{
		return false;
	}


	m_Socket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == m_Socket)
	{
		return false;
	}
	
	if (NULL == CreateIoCompletionPort((HANDLE)m_Socket, m_hIOCompletionPort, (DWORD)&m_Socket, 0))
	{
		closesocket(m_Socket);
		return false;
	}
	

	ZeroMemory((char *)&ServerAddress, sizeof(ServerAddress));
	ServerAddress.sin_family = AF_INET;                      
	ServerAddress.sin_addr.s_addr = INADDR_ANY;
	ServerAddress.sin_port = htons(Port);

	if (SOCKET_ERROR == bind(m_Socket, (struct sockaddr *) &ServerAddress, sizeof(ServerAddress)))
	{
		return false;
	}

	if (SOCKET_ERROR == listen(m_Socket, SOMAXCONN))
	{
		return false;
	}

	DWORD dwBytes = 0;
	if (SOCKET_ERROR == WSAIoctl(
		m_Socket,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidAcceptEx,
		sizeof(GuidAcceptEx),
		&m_lpfnAcceptEx,
		sizeof(m_lpfnAcceptEx),
		&dwBytes,
		NULL,
		NULL))
	{
		CloseHandle(m_hIOCompletionPort);
		closesocket(m_Socket);
		return false;
	}

	// 获取GetAcceptExSockAddrs函数指针，也是同理
	if (SOCKET_ERROR == WSAIoctl(
		m_Socket,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidGetAcceptExSockAddrs,
		sizeof(GuidGetAcceptExSockAddrs),
		&m_lpfnGetAcceptExSockAddrs,
		sizeof(m_lpfnGetAcceptExSockAddrs),
		&dwBytes,
		NULL,
		NULL))
	{
		CloseHandle(m_hIOCompletionPort);
		closesocket(m_Socket);
		return false;
	}

	return true;
}

bool Cube_TCPFastIO_Client::Start(int ThreadProcessCore)
{
	Stop();

	WSADATA wsaData;

	m_ShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_ShutdownEvent == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	m_hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if (NULL == m_hIOCompletionPort)
	{
		goto ERR;
	}

	int nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (NO_ERROR != nResult)
	{
		goto ERR;
	}
	m_Socket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == m_Socket)
	{
		return false;
	}

	if (NULL == CreateIoCompletionPort((HANDLE)m_Socket, m_hIOCompletionPort, (DWORD)&m_Socket, 0))
	{
		closesocket(m_Socket);
		return false;
	}

	SYSTEM_INFO si;

	GetSystemInfo(&si);

	m_ThreadCount = CUBE_FASTIO_THREAD_PROCESSER_PERCOUNT * si.dwNumberOfProcessors;

	//Create threads
	if (ThreadProcessCore != 0)
	{
		m_ThreadCount = ThreadProcessCore;
	}

	m_ClientProcessors = new Cube_TCPFastIO_ClientProcessor[m_ThreadCount];

	if (m_ClientProcessors == NULL)
	{
		goto ERR;
	}
	for (int i = 0; i < m_ThreadCount; i++)
	{
		m_ClientProcessors[i].SetServerPointer(this);
	}
	for (int i = 0; i < m_ThreadCount; i++)
	{
		m_ClientProcessors[i].start();
	}
	return true;
ERR:
	return false;
}

void Cube_TCPFastIO_Client::Stop()
{
	if(m_ShutdownEvent!=INVALID_HANDLE_VALUE)
	SetEvent(m_ShutdownEvent);

	for (int i = 0; i < m_ThreadCount; i++)
	{
		PostQueuedCompletionStatus(m_hIOCompletionPort, 0, CUBE_FASTIO_EXIT_CODE, NULL);
	}
	for (int i = 0; i < m_ThreadCount; i++)
	{
		if(m_ClientProcessors[i].IsRunning())
		m_ClientProcessors[i].WaitForExit();
	}
	FreeSocket();
}


void Cube_TCPFastIO_Client::InitializeNetwork()
{

}

void Cube_TCPFastIO_Client::FreeNetwork()
{
	WSACleanup();
}

bool Cube_TCPFastIO_Client::Connect(const char *addr, int port,int ProcessorsCount)
{
	struct sockaddr_in ServerAddress;
	struct hostent *Server;
	if (!Start(ProcessorsCount))
	{
		return false;
	}

	Server = gethostbyname(addr);
	if (Server == NULL)
	{
		Stop();
		return false;
	}


	ZeroMemory((char *)&ServerAddress, sizeof(ServerAddress));
	ServerAddress.sin_family = AF_INET;
	CopyMemory((char *)&ServerAddress.sin_addr.s_addr,
		(char *)Server->h_addr,
		Server->h_length);

	ServerAddress.sin_port = htons(port);

	int ret = connect(m_Socket, (struct sockaddr *)&ServerAddress, sizeof(SOCKADDR_IN));
	if (ret == SOCKET_ERROR)
	{
		m_IsConnecting=false;
		Stop();
		return false;
	}
	m_IsConnecting=true;
	for (int i=0;i<m_ThreadCount;i++)
	{
		_PostRecv(mallocNewContext());
	}
	return true;
}

bool Cube_TCPFastIO_Client::Connect(pt_ulong ulIP, int port,int ProcessorsCount)
{
	SOCKADDR_IN sockaddr_in;
	sockaddr_in.sin_family = AF_INET;
	sockaddr_in.sin_addr.s_addr = ulIP;
	sockaddr_in.sin_port = htons(port);
	return Connect(sockaddr_in,ProcessorsCount);
}

bool Cube_TCPFastIO_Client::Connect(SOCKADDR_IN addrin,int ProcessorsCount)
{
	if(!Start(ProcessorsCount))
		return false;
	int ret = connect(m_Socket, (struct sockaddr *)&addrin, sizeof(SOCKADDR_IN));
	if (ret == SOCKET_ERROR)
	{
		Stop();
		m_IsConnecting=false;
		return false;
	}
	m_IsConnecting=true;
	for (int i=0;i<m_ThreadCount;i++)
	{
		_PostRecv(mallocNewContext());
	}
	return true;
}


void Cube_TCPFastIO_Client::Disconnect()
{
	closesocket(m_Socket);
	m_Socket=INVALID_SOCKET;
	m_IsConnecting=false;
}

void Cube_TCPFastIO_Client::FreeSocket()
{
	while (m_ContextListLast != NULL)
	{
		IO_CONTEXT *tempPointer = m_ContextListLast->m_pPreviousContext;
		delete m_ContextListLast;
		m_ContextListLast = tempPointer;
	}

	delete[] m_ClientProcessors;

	if(m_ShutdownEvent!=INVALID_HANDLE_VALUE)
	CloseHandle(m_ShutdownEvent);

	if (m_Socket!=INVALID_SOCKET)
	{
		closesocket(m_Socket);
		m_Socket=INVALID_SOCKET;
	}

	m_ContextListLast=NULL;
	m_Socket=INVALID_SOCKET;
	m_ThreadCount=0;
	m_ShutdownEvent=INVALID_HANDLE_VALUE;
	m_ClientProcessors=NULL;
}

BOOL Cube_TCPFastIO_Client::IsConnecting()
{
	return m_IsConnecting;
}

HANDLE Cube_TCPFastIO_Client::GetIOCompletedPort()
{
	return m_hIOCompletionPort;
}

HANDLE Cube_TCPFastIO_Client::GetShutdownEvent()
{
	return m_ShutdownEvent;
}

SOCKET Cube_TCPFastIO_Client::GetSocket()
{
	return m_Socket;
}

void Cube_TCPFastIO_Client::OnDisconnectEvent(int flag)
{
	if (m_IsConnecting)
	{
		m_IsConnecting=false;
		OnDisconnected(flag);
		Disconnect();
	}
}

bool Cube_TCPFastIO_Client::Send( pt_byte *Buffer, pt_ulong Size)
{
	if (send(m_Socket, Buffer, Size, 0) == SOCKET_ERROR) return false;
	return true;
}

bool Cube_TCPFastIO_Client::_PostRecv(IO_CONTEXT *context)
{
	DWORD dwFlags = 0;
	DWORD dwBytes = 0;


	WSABUF *p_wbuf = &context->m_wsaBuf;
	OVERLAPPED *p_ol = &context->m_Overlapped;
	context->m_OpType = RECV_POSTED;

	// 初始化完成后，，投递WSARecv请求
	int nBytesRecv = WSARecv(context->m_socket, p_wbuf, 1, &dwBytes, &dwFlags, p_ol, NULL);

	// 如果返回值错误，并且错误的代码并非是Pending的话，那就说明这个重叠请求失败了
	if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
	{
		return false;
	}
	return true;
}

IO_CONTEXT * Cube_TCPFastIO_Client::mallocNewContext()
{
	IO_CONTEXT *NewContext = new IO_CONTEXT;
	if (NewContext == NULL)
	{
		return NULL;
	}
	if (m_ContextListLast == NULL)
	{
		NewContext->Reset();
		m_ContextListLast = NewContext;
	}
	else
	{
		NewContext->Reset();
		NewContext->m_pPreviousContext = m_ContextListLast;
		m_ContextListLast = NewContext;
	}
	NewContext->m_Flag = 0;
	NewContext->m_socket = m_Socket;
	return NewContext;
}

void Cube_TCPFastIO_Client::RemoveContext(IO_CONTEXT *context)
{
	if (context->m_pPreviousContext != NULL)
	{
		context->m_pPreviousContext->m_pNextContext = context->m_pNextContext;
		if (context->m_pNextContext != NULL)
		{
			context->m_pNextContext->m_pPreviousContext = context->m_pPreviousContext;
		}
	}
	else
	{
		m_ContextListLast = context->m_pNextContext;
	}
	delete context;
}



void Cube_TCPFastIO_ClientProcessor::run()
{
	OVERLAPPED						*pOverlapped = NULL;
	DWORD							dwBytesTransfered = 0;
	pt_int							ThreadOpCode,flag;

   while (WAIT_OBJECT_0 != WaitForSingleObject(m_pcCube_TCPFastIO_Client->GetShutdownEvent(), 0))
	{
	BOOL bReturn = GetQueuedCompletionStatus(
		m_pcCube_TCPFastIO_Client->GetIOCompletedPort(),
		&dwBytesTransfered,
		(PULONG_PTR)&ThreadOpCode,
		&pOverlapped,
		INFINITE);

	if (ThreadOpCode == CUBE_FASTIO_EXIT_CODE)
	{
		break;
	}
	IO_CONTEXT* pIoContext = CONTAINING_RECORD(pOverlapped, IO_CONTEXT, m_Overlapped);

	if (!bReturn)
	{
		DWORD dwErr = GetLastError();

		if (ERROR_NETNAME_DELETED == dwErr)
		{
			m_pcCube_TCPFastIO_Client->OnDisconnectEvent(pIoContext->m_Flag);
			break;
		}
		else
		{
			m_pcCube_TCPFastIO_Client->OnDisconnectEvent(pIoContext->m_Flag);
			goto ERR;
		}
		continue;
	}
	else
	{

		if ((0 == dwBytesTransfered) && (RECV_POSTED == pIoContext->m_OpType || SEND_POSTED == pIoContext->m_OpType))
		{
			m_pcCube_TCPFastIO_Client->OnDisconnectEvent(pIoContext->m_Flag);
			break;
		}
		else
		{
			switch (pIoContext->m_OpType)
			{
			case RECV_POSTED:
			{
				m_pcCube_TCPFastIO_Client->OnReceived(pIoContext->m_Flag, pIoContext->m_IOBuffer, dwBytesTransfered);
				m_pcCube_TCPFastIO_Client->_PostRecv(pIoContext);
			}
			break;
			case SEND_POSTED:
			{
				//nop
			}
			break;
			default:
				flag=(pIoContext->m_Flag);
				break;
			} //switch
		}//if
	}//if

}//while

return;
ERR:
m_pcCube_TCPFastIO_Client->OnError();
return;
}
