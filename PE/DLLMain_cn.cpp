#define DLL_IMPLEMENT
#include "_DllExport.h"

///////////////////////////////////////////////////////////////////////////////
//Global
//////////////////////////////////////////////////////////////////////////
HANDLE G_hMutex;

static char HookMessage[PEDOLL_HOOKMESSAGE_MAXLEN];
PEDoll_NetWork				g_PeDollNetwork;
APIHooker					sendHooker;
APIHooker					NtDeviceIoControlFileHooker;
APIHooker					g_binHooker;
PEDoll_SendStackThread		g_SendStack;
int							g_binaryHookStackSize;
int                         g_stackShowData[32];
int							g_binaryRteax;
static const int hex_to_dex_table[]={0,0,0,0,0,0,0,0,0,  
	0,0,0,0,0,0,0,0,0,  
	0,0,0,0,0,0,0,0,0,  
	0,0,0,0,0,0,0,0,0,  
	0,0,0,0,0,0,0,0,0,  
	0,0,0,0,1,2,3,4,5,6,  
	7,8,9,0,0,0,0,0,0,  
	0,10,11,12,13,14,15,0,0,  
	0,0,0,0,0,0,0,0,0,  
	0,0,0,0,0,0,0,0,0,  
	0,0,0,0,0,0,10,  
	11,12,13,14,15};

unsigned int hex_to_decimal(char* hex_str)   
{  
	char ch;   
	unsigned int iret=0;  
	while(ch=*hex_str++)   
	{   
		iret=(iret<<4)|hex_to_dex_table[ch];  
	}   
	return iret;   
} 
void HookFunctionCommand(char *Cmd,bool bHook);
void HookFunction(char *LibraryName,char *FunctionName,DWORD func,bool bHook,APIHooker *pHooker);
void BinaryCommand(char *strbin,char *stackbin);
int MonitorQuery(char *APIInfo,unsigned int esp);
DWORD getEsp();

extern "C"
{
volatile void BinHook()
{
	static DWORD addr;
	static DWORD temp;
	static DWORD ioft;
	static DWORD iesp,iebp,retaddr;

	_asm mov iesp,esp
	_asm mov iebp,ebp
	iesp+=4;
	retaddr=*((DWORD *)iesp);
	ioft=0;
	sprintf(HookMessage,"Binary Hooker-->");

	while (g_stackShowData[ioft]!=-1&&ioft<32)
	{
		temp=g_stackShowData[ioft+1];
		if (g_stackShowData[ioft]==1)//string
		{
			temp=iesp+temp;
			addr=*((DWORD *)temp);
			sprintf(HookMessage,"%s string (%s)",HookMessage,(char *)addr);
		}
		else if(g_stackShowData[ioft]==2)
		{
			temp=iesp+temp;
			addr=*((DWORD *)temp);
			sprintf(HookMessage,"%s decimal (%d)",HookMessage,addr);
		}
		else if(g_stackShowData[ioft]==3)
		{
			temp=iebp+temp;
			addr=*((DWORD *)temp);
			sprintf(HookMessage,"%s string (%s)",HookMessage,(char *)addr);
		}
		else if(g_stackShowData[ioft]==4)
		{
			temp=iebp+temp;
			addr=*((DWORD *)temp);
			sprintf(HookMessage,"%s decimal (%d)",HookMessage,addr);
		}
		ioft+=2;
	}
	
	g_PeDollNetwork.SendControllerMessage(HookMessage);

	_asm
	{
		
		mov esp,iesp;
		add esp,g_binaryHookStackSize;
		mov eax,retaddr;
		push eax;
		mov eax,g_binaryRteax;
		ret;
	}
	
}
}
DWORD getEsp()
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	return dwesp;
}

void ReadAddressInfo(DWORD Address,int size)
{
	DWORD Old,dummy,readsize;
	if (size>PEDOLL_STACK_MEMORY_LEN)
	{
		size=PEDOLL_STACK_MEMORY_LEN;
	}
	

	PEDOLL_MEMORY_ACK_PACKET ackPacket;
	ackPacket.Address=Address;
	ackPacket.size=0;
	if(ReadProcessMemory(GetCurrentProcess(),(LPCVOID)Address,ackPacket.buffer,size,&readsize))
	{
		ackPacket.size=readsize;
	}
	else
	{
		memset(ackPacket.buffer,0,sizeof ackPacket.buffer);
		ackPacket.size=0;
	}
	
	g_PeDollNetwork.SendAckPack((char *)&ackPacket,sizeof ackPacket);
}

BOOL IsAlreadyRunning()
{
	G_hMutex = ::CreateMutex(NULL,TRUE,REMOTESHELL_PROCESS_GUID);

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(G_hMutex);
		return TRUE;
	}
	return FALSE;
}

char *GetProcName()
{
	static char Path[MAX_PATH];
	GetModuleFileNameA(GetModuleHandle(NULL),Path,MAX_PATH);
	int oft=strlen(Path);
	while(oft)
	{
		if (Path[oft]=='/'||Path[oft]=='\\')
		{
			break;
		}
		oft--;
	}
	return Path+oft+1;
}


PEDoll_NetWork::PEDoll_NetWork()
{
	m_stID=1;
	InitializeCriticalSection(&m_APIQueryCS);
	m_Isconnect=FALSE;
	m_NetWorkConnectEvent=CreateEvent(NULL,TRUE,FALSE,NULL);

#define GRAMMAR_TOKEN_NUMBER 1
	m_Lexer.RegisterSpacer(' ');
	m_Lexer.RegisterSpacer('\r');
	m_Lexer.RegisterSpacer('\n');
	unsigned int ContainerIndex=m_Lexer.RegisterContainer('<','>');

	m_Grammar.SetLexer(&m_Lexer);

	CubeBlockType Contain=CubeBlockType(NULL,2,GRAMMAR_CONATINER,ContainerIndex);
	CubeBlockType Param=CubeBlockType(NULL,1,GRAMMAR_TYPE_ANY);
	CubeBlockType Spacer=CubeBlockType(NULL,1,GRAMMAR_SPACER);
	CubeBlockType Number=CubeBlockType("[0-9]+",2,GRAMMAR_TOKEN,GRAMMAR_TOKEN_NUMBER);
	Number.AsRegex();

	m_Grammar.RegisterBlockType(Contain);
	m_Grammar.RegisterBlockType(Number);
	m_Grammar.RegisterBlockType(Param);
	m_Grammar.RegisterDiscard(Spacer);

	CubeBlockType cbt_hook=CubeBlockType(PEDOLL_DOOL_CMD_HOOK_STR,2,GRAMMAR_TOKEN,PEDOLL_DOOL_CMD_HOOK);
	m_Grammar.RegisterBlockType(cbt_hook);

	CubeBlockType cbt_unhook=CubeBlockType(PEDOLL_DOOL_CMD_UNHOOK_STR,2,GRAMMAR_TOKEN,PEDOLL_DOOL_CMD_UNHOOK);
	m_Grammar.RegisterBlockType(cbt_unhook);

	CubeBlockType cbt_terminate=CubeBlockType(PEDOLL_DOOL_CMD_TERMINATE_STR,2,GRAMMAR_TOKEN,PEDOLL_DOOL_CMD_TERMINATE);
	m_Grammar.RegisterBlockType(cbt_terminate);

	CubeBlockType cbt_run=CubeBlockType(PEDOLL_DOOL_CMD_RUN_STR,2,GRAMMAR_TOKEN,PEDOLL_DOOL_CMD_RUN);
	m_Grammar.RegisterBlockType(cbt_run);

	CubeBlockType cbt_esp=CubeBlockType(PEDOLL_DOLL_CMD_READESP_STR,2,GRAMMAR_TOKEN,PEDOLL_DOOL_CMD_READESP);
	m_Grammar.RegisterBlockType(cbt_esp);

	CubeBlockType cbt_readmemory=CubeBlockType(PEDOLL_DOLL_CMD_READ_MEMORY_STR,2,GRAMMAR_TOKEN,PEDOLL_DOOL_CMD_READMEMORY);
	m_Grammar.RegisterBlockType(cbt_readmemory);

	CubeBlockType cbt_binary=CubeBlockType(PEDOLL_DOLL_CMD_HOOK_BIN_STR,2,GRAMMAR_TOKEN,PEDOLL_DOOL_CMD_BINARY);
	m_Grammar.RegisterBlockType(cbt_binary);

	CubeBlockType cbt_unbinary=CubeBlockType(PEDOLL_DOLL_CMD_HOOK_UNBIN_STR,2,GRAMMAR_TOKEN,PEDOLL_DOOL_CMD_UNBINARY);
	m_Grammar.RegisterBlockType(cbt_unbinary);

	CubeGrammarSentence Sen;
	Sen.Reset();
	Sen.add(cbt_hook);
	Sen.add(Param);
	m_Instr_Hook=m_Grammar.RegisterSentence(Sen);

	Sen.Reset();
	Sen.add(cbt_unhook);
	Sen.add(Param);
	m_Instr_Unhook=m_Grammar.RegisterSentence(Sen);

	Sen.Reset();
	Sen.add(cbt_terminate);
	m_Instr_Terminate=m_Grammar.RegisterSentence(Sen);

	Sen.Reset();
	Sen.add(cbt_run);
	m_Instr_Run=m_Grammar.RegisterSentence(Sen);

	Sen.Reset();
	Sen.add(cbt_readmemory);
	Sen.add(Param);
	Sen.add(Param);
	m_Instr_readMemory=m_Grammar.RegisterSentence(Sen);

	Sen.Reset();
	Sen.add(cbt_esp);
	m_Instr_esp=m_Grammar.RegisterSentence(Sen);

	Sen.Reset();
	Sen.add(cbt_binary);
	Sen.add(Param);
	Sen.add(Param);
	Sen.add(Param);
	Sen.add(Param);
	m_Instr_binary=m_Grammar.RegisterSentence(Sen);

	Sen.Reset();
	Sen.add(cbt_unbinary);
	m_Instr_unbinary=m_Grammar.RegisterSentence(Sen);
}

void PEDoll_NetWork::OnConnected(IN SOCKET socket,IN SOCKADDR_IN addrin,OUT pt_int &Flag)
{
	m_cursocket=socket;
	m_Isconnect=TRUE;
	SetEvent(m_NetWorkConnectEvent);
	g_SendStack.start();
}
void PEDoll_NetWork::SendAckPack(char *buf,int Size)
{
	g_SendStack.PushSendPacket(buf,Size);
}

bool PEDoll_NetWork::SocketSend(char *buf,int Size)
{
	return Send(m_cursocket,buf,Size);
}

void PEDoll_NetWork::WaitForConnect()
{
	OVERLAPPED						*pOverlapped = NULL;
	DWORD							dwBytesTransfered = 0;
	pt_int							ThreadOpCode;
	while (TRUE)
	{
		BOOL bReturn = GetQueuedCompletionStatus(
			GetIOCompletedPort(),
			&dwBytesTransfered,
			(PULONG_PTR)&ThreadOpCode,
			&pOverlapped,
			INFINITE);

		IO_CONTEXT* pIoContext = CONTAINING_RECORD(pOverlapped, IO_CONTEXT, m_Overlapped);

		if(pIoContext->m_OpType==ACCEPT_POSTED)
		{
			SOCKADDR_IN* ClientAddr = NULL;
			SOCKADDR_IN* LocalAddr = NULL;
			pt_int		 __flag;
			int remoteLen = sizeof(SOCKADDR_IN), localLen = sizeof(SOCKADDR_IN);
			((LPFN_GETACCEPTEXSOCKADDRS)(GetAcceptSockAddrsInterface()))(pIoContext->m_wsaBuf.buf, /*CUBE_FASTIO_BUFFER_LEN - ((sizeof(SOCKADDR_IN) + 16) * 2)*/0,
				sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, (LPSOCKADDR*)&LocalAddr, &localLen, (LPSOCKADDR*)&ClientAddr, &remoteLen);
			ConnectEvent(pIoContext->m_socket,*ClientAddr,__flag);
			HANDLE hTemp = CreateIoCompletionPort((HANDLE)pIoContext->m_socket, GetIOCompletedPort(),0, 0);
			if (NULL == hTemp)
			{
				goto ERR;
			}
			IO_CONTEXT *pNewContext = mallocNewContext();
			if (pNewContext==NULL)
			{
				goto ERR;
			}

			pNewContext->m_addrin = *ClientAddr;
			pNewContext->m_Flag = __flag;
			pNewContext->m_socket = pIoContext->m_socket;

			if (false == _PostRecv(pNewContext))
			{
				goto ERR;
			}
			RemoveContext(pIoContext);
			break;
		}
	}//while
	return;
ERR:
	exit(0);
	return;
}
void PEDoll_NetWork::Preload()
{
	OVERLAPPED						*pOverlapped = NULL;
	DWORD							dwBytesTransfered = 0;
	pt_int							ThreadOpCode;
	g_SendStack.PerloadExecute();
	while (TRUE)
	{
		BOOL bReturn = GetQueuedCompletionStatus(
			GetIOCompletedPort(),
			&dwBytesTransfered,
			(PULONG_PTR)&ThreadOpCode,
			&pOverlapped,
			INFINITE);
		if (ThreadOpCode==CUBE_FASTIO_EXIT_CODE)
		{
			exit(0);
		}
		IO_CONTEXT* pIoContext = CONTAINING_RECORD(pOverlapped, IO_CONTEXT, m_Overlapped);

		if (!bReturn)
		{
			DWORD dwErr = GetLastError();

			if (ERROR_NETNAME_DELETED == dwErr)
			{
				DisconnectEvent(pIoContext->m_Flag);
				continue;
			}
			else
			{
				exit(0);
			}
			continue;
		}
		else
		{

			if ((0 == dwBytesTransfered) && (RECV_POSTED == pIoContext->m_OpType || SEND_POSTED == pIoContext->m_OpType))
			{
				DisconnectEvent(pIoContext->m_Flag);
				continue;
			}
			else
			{
				if (pIoContext->m_OpType==RECV_POSTED)
				{
					PEDOLL_COMMAND_PACKET *cmdPack=(PEDOLL_COMMAND_PACKET *)pIoContext->m_IOBuffer;
					if (ParserCommand(cmdPack->Info)==m_Instr_Run)
					{
						_PostRecv(pIoContext);
						break;
					}
					_PostRecv(pIoContext);
					g_SendStack.PerloadExecute();
				} //switch
			}//if
		}//if
	}//while

	return;
}
unsigned int PEDoll_NetWork::ParserCommand(char *Command)
{
	CubeGrammarSentence sen;
	m_Lexer.SortText(Command);
	unsigned int SenType;
	SenType=m_Grammar.GetNextInstr(sen);
	if (SenType==GRAMMAR_SENTENCE_UNKNOW||SenType==GRAMMAR_SENTENCE_END)
	{
		return SenType;
	}

	if (SenType==m_Instr_Hook)
	{
		HookFunctionCommand(sen.GetBlockString(1),true);
	}

	if (SenType==m_Instr_Unhook)
	{
		HookFunctionCommand(sen.GetBlockString(1),false);
	}

	if (SenType==m_Instr_Terminate)
	{
		PEDOLL_ACK_PACKET ack;
		SendAckPack((pt_byte *)&ack,sizeof ack);
		exit(0);
	}

	if (SenType==m_Instr_Run)
	{
		PEDOLL_ACK_PACKET ack;
		SendAckPack((pt_byte *)&ack,sizeof ack);
	}

	if (SenType==m_Instr_esp)
	{
		PEDOLL_ACK_PACKET ack;
		ack.Return=getEsp();
		SendAckPack((pt_byte *)&ack,sizeof ack);
	}

	if (SenType==m_Instr_readMemory)
	{
		ReadAddressInfo(atol(sen.GetBlockString(1)),atoi(sen.GetBlockString(2)));
	}

	if (SenType==m_Instr_unbinary)
	{
		g_binHooker.Unhook();
		PEDOLL_ACK_PACKET ack;
		ack.Return=1;
		SendAckPack((pt_byte *)&ack,sizeof ack);
	}

	if (SenType==m_Instr_binary)
	{
		static char BinHookBin[256];
		char *pBinary;
		char hex[3];
		char Num[16];
		int oft=0;
		int binsize;
		int g_dataOft=0;
		g_binHooker.Unhook();
		if((strlen(sen.GetBlockString(1))&1)||strlen(sen.GetBlockString(1))>512)
		{
			PEDOLL_ACK_PACKET ack;
			ack.Return=0;
			SendAckPack((pt_byte *)&ack,sizeof ack);
		}
		else
		{
			pBinary=sen.GetBlockString(1);
			while (*pBinary)
			{
				hex[0]=*pBinary;
				hex[1]=*(pBinary+1);
				hex[2]=0;

				BinHookBin[oft++]=hex_to_decimal(hex);

				pBinary+=2;
			}
			binsize=oft;
			g_binaryHookStackSize=atoi(sen.GetBlockString(2));
			pBinary=sen.GetBlockString(3);
			g_binaryRteax=atoi(sen.GetBlockString(4));
			while (*pBinary)
			{
				if (*pBinary=='S')
				{
					g_stackShowData[g_dataOft]=1;
				}
				else if(*pBinary=='D')
				{
					g_stackShowData[g_dataOft]=2;
				}
				else if(*pBinary=='B')
				{
					g_stackShowData[g_dataOft]=3;
				}
				else if(*pBinary=='E')
				{
					g_stackShowData[g_dataOft]=4;
				}
				else
				{
					PEDOLL_ACK_PACKET ack;
					ack.Return=0;
					SendAckPack((pt_byte *)&ack,sizeof ack);
					return SenType;
				}
				pBinary++;
				oft=0;
				while (TRUE)
				{
					if (*pBinary>='0'&&*pBinary<='9')
					{
						Num[oft++]=*pBinary;
					}
					else
					{
						Num[oft]=0;
						break;
					}
					if (oft>=16)
					{
						PEDOLL_ACK_PACKET ack;
						ack.Return=0;
						SendAckPack((pt_byte *)&ack,sizeof ack);
						return SenType;
					}
					pBinary++;
				}
				g_dataOft++;
				g_stackShowData[g_dataOft]=atoi(Num);
				g_dataOft++;
			}
			g_stackShowData[g_dataOft]=-1;

			if(!g_binHooker.Hook(BinHookBin,binsize,(DWORD)BinHook))
			{
				PEDOLL_ACK_PACKET ack;
				ack.Return=0;
				SendAckPack((pt_byte *)&ack,sizeof ack);
			}
			else
			{
				PEDOLL_ACK_PACKET ack;
				ack.Return=1;
				SendAckPack((pt_byte *)&ack,sizeof ack);
			}
			

		}
	}

	return SenType;
}
void PEDoll_NetWork::OnReceived(IN pt_int flag, IN pt_byte *Buffer, IN int Size)
{
	switch(*(DWORD *)Buffer)
	{
	case PEDOLL_ACK_MAGICNUMBER:
		g_SendStack.OnAck();
		return;
		break;
	case PEDOLL_CMD_MAGICNUMBER:
		{
			PEDOLL_COMMAND_PACKET *cmdPack=(PEDOLL_COMMAND_PACKET *)Buffer;
			if(ParserCommand(cmdPack->Info)==m_Instr_Run)
			{
				PEDOLL_API_ACK ack;
				ack.Exec=PEDOLL_EXECUTE_PASS;
				ack.MagicNumber=0;
				ack.PacketID=0;
				EnterCriticalSection(&m_APIQueryCS);
				m_vAPIExec.push_back(ack);
				LeaveCriticalSection(&m_APIQueryCS);
			}
		}
		break;
	case PEDOLL_APIACK_MAGICNUMBER:
		{
			EnterCriticalSection(&m_APIQueryCS);
			m_vAPIExec.push_back(*(PEDOLL_API_ACK *)Buffer);
			LeaveCriticalSection(&m_APIQueryCS);
		}
		break;
	}
}
void PEDoll_NetWork::SendDumpPack(char *Info,char *buffer,int Len)
{
	if (!m_Isconnect)
	{
		return;
	}
	char *TempBuffer=new char[Len+sizeof(PEDOLL_DUMP_PACKET_HEADER)];
	if (!TempBuffer)
	{
		return;
	}
	PEDOLL_DUMP_PACKET_HEADER *Header=(PEDOLL_DUMP_PACKET_HEADER *)TempBuffer;
	Header->MagicNumber=PEDOLL_DUMP_MAGICNUMBER;
	strcpy(Header->DestInfo,Info);
	memcpy(TempBuffer+sizeof(PEDOLL_DUMP_PACKET_HEADER),buffer,Len);
	SendAckPack(TempBuffer,Len+sizeof(PEDOLL_DUMP_PACKET_HEADER));
	delete [] TempBuffer;
}
int PEDoll_NetWork::GetFreeCmdID()
{
	int ID;
	EnterCriticalSection(&m_APIQueryCS);
	ID=m_stID;
	m_stID++;
	LeaveCriticalSection(&m_APIQueryCS);
	return ID;
}
BOOL PEDoll_NetWork::QueryCommandPack(unsigned int PacketID,PEDOLL_API_ACK &pack)
{
	BOOL ret=FALSE;
	EnterCriticalSection(&m_APIQueryCS);
	for (unsigned int i=0;i<m_vAPIExec.size();i++)
	{
		if (m_vAPIExec[i].PacketID==PacketID||m_vAPIExec[i].MagicNumber==0)
		{
			pack=m_vAPIExec[i];
			ret=TRUE;
			m_vAPIExec.erase(m_vAPIExec.begin()+i);
			break;
		}
	}
	LeaveCriticalSection(&m_APIQueryCS);
	return ret;
}
void PEDoll_NetWork::OnDisconnected(IN pt_int flag)
{
	exit(0);
}
void PEDoll_NetWork::SendControllerMessage(char *message)
{
	PEDOLL_MESSAGE_PACKET pack;
	strcpy(pack.Messages,message);
	SendMessagePack(pack);
}
void PEDoll_NetWork::SendMessagePack(PEDOLL_MESSAGE_PACKET pack)
{
	SendAckPack((pt_byte *)&pack,sizeof(PEDOLL_MESSAGE_PACKET));
}
void PEDoll_NetWork::SendHookInfoPack(PEDOLL_HOOKINFO_PACKET pack)
{
	SendAckPack((pt_byte *)&pack,sizeof(PEDOLL_HOOKINFO_PACKET));
}


PEDoll_SendStackThread::PEDoll_SendStackThread()
{
	m_bRun=true;
	InitializeCriticalSection(&m_QueueCS);
	m_SendEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	m_ackPackEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
}

void PEDoll_SendStackThread::ThreadExit()
{
	m_bRun=false;
	SetEvent(m_SendEvent);
	WaitForExit();
}

void PEDoll_SendStackThread::OnAck()
{
	SetEvent(m_ackPackEvent);
}

void PEDoll_SendStackThread::PushSendPacket(void *buffer,unsigned int Len)
{	
	if (!g_PeDollNetwork.m_Isconnect)
	{
		exit(0);
	}
	SEND_PACKET_INFO SendPacketInfo;
	SendPacketInfo.buffer=new char[Len];
	if (SendPacketInfo.buffer==NULL)
	{
		return;
	}
	memcpy(SendPacketInfo.buffer,buffer,Len);
	SendPacketInfo.Size=Len;
	EnterCriticalSection(&m_QueueCS);
	m_PacketQueue.push(SendPacketInfo);
	LeaveCriticalSection(&m_QueueCS);

	SetEvent(m_SendEvent);
}

void PEDoll_SendStackThread::run()
{
	bool connectBool=false;
	while (m_bRun)
	{
		WaitForSingleObject(m_SendEvent,INFINITE);
		while (!m_PacketQueue.empty())
		{
			SEND_PACKET_INFO SendPacketInfo;
			
			EnterCriticalSection(&m_QueueCS);
			ResetEvent(m_ackPackEvent);
			SendPacketInfo=m_PacketQueue.front();
			m_PacketQueue.pop();
			NtDeviceIoControlFileHooker.Unhook();
			sendHooker.Unhook();
			connectBool=g_PeDollNetwork.SocketSend((char *)SendPacketInfo.buffer,SendPacketInfo.Size);
			sendHooker.Rehook();
			NtDeviceIoControlFileHooker.Rehook();
			WaitForSingleObject(m_ackPackEvent,50);

			LeaveCriticalSection(&m_QueueCS);
			Sleep(10);
			SendPacketInfo.free();
			if (!connectBool)
			{
				exit(0);
			}
			
		}
	}
}


void PEDoll_SendStackThread::PerloadExecute()
{
	bool connectBool=false;
	while (!m_PacketQueue.empty())
	{
		SEND_PACKET_INFO SendPacketInfo;

		EnterCriticalSection(&m_QueueCS);
		SendPacketInfo=m_PacketQueue.front();
		m_PacketQueue.pop();
		NtDeviceIoControlFileHooker.Unhook();
		sendHooker.Unhook();
		connectBool=g_PeDollNetwork.SocketSend((char *)SendPacketInfo.buffer,SendPacketInfo.Size);
		sendHooker.Rehook();
		NtDeviceIoControlFileHooker.Rehook();
		LeaveCriticalSection(&m_QueueCS);
		SendPacketInfo.free();
		if (!connectBool)
		{
			exit(0);
		}
		Sleep(100);
	}
}


bool GetAddressBySocket(SOCKET m_socket,SOCKADDR_IN &m_address)
{
	memset(&m_address, 0, sizeof(m_address));
	int nAddrLen = sizeof(m_address);

	if(::getpeername(m_socket, (SOCKADDR*)&m_address, &nAddrLen) != 0)
	{
		return false;
	}
	return true;
}


int MonitorQuery(char *APIInfo,unsigned int esp)
{
	PEDOLL_HOOKINFO_PACKET HookInfoPacket;
	HookInfoPacket.PacketID=g_PeDollNetwork.GetFreeCmdID();
	strcpy(HookInfoPacket.APIInfo,APIInfo);
	HookInfoPacket.esp=esp;
	g_PeDollNetwork.SendHookInfoPack(HookInfoPacket);

	PEDOLL_API_ACK cmdPack;
	while (TRUE)
	{
		if(g_PeDollNetwork.QueryCommandPack(HookInfoPacket.PacketID,cmdPack))
			break;
		Sleep(10);
	}
	return (cmdPack.Exec);
}


string UnicodeToANSI( LPCWSTR str )
{
	USES_CONVERSION;
	std::string  astr( W2A(str)) ;
	return astr;
}

void DLL_API X_dummy()
{
	return;
}

HANDLE g_hPipe=INVALID_HANDLE_VALUE;

BOOL InitializeNetwork()
{
	return g_PeDollNetwork.Start(PEDOLL_DOLL_NETWORK_PORT,1,1);
}


PROCESSENTRY32 GetProcessNameByPID(DWORD PID)
{
	PROCESSENTRY32 procEntry = { 0 };
	strcpy(procEntry.szExeFile,"UNKNOW");
	HANDLE procSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(procSnap == INVALID_HANDLE_VALUE)
	{
		return procEntry;
	}

	procEntry.dwSize = sizeof(PROCESSENTRY32);
	BOOL bRet = Process32First(procSnap,&procEntry);
	while(bRet)
	{
		if (procEntry.th32ProcessID==PID)
		{
			break;
		}
		bRet = Process32Next(procSnap,&procEntry);
	}

	CloseHandle(procSnap);
	return procEntry;
}


APIHooker WriteProcessMemoryHooker;

int APIENTRY hook_WriteProcessMemory( __in HANDLE hProcess, __in LPVOID lpBaseAddress, __in_bcount(nSize) LPCVOID lpBuffer, __in SIZE_T nSize, __out_opt SIZE_T * lpNumberOfBytesWritten )
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}

	sprintf(HookMessage, "%s <%08X> <%08X> <%08X> <%d>",PEDOLL_QUERY_WRITEPROCESSMEMORY,(DWORD)hProcess,(int)lpBaseAddress,(int)lpBuffer,nSize);

	int reply=MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		WriteProcessMemoryHooker.Unhook();
		int st = WriteProcessMemory(hProcess, lpBaseAddress, lpBuffer,nSize,lpNumberOfBytesWritten);
		WriteProcessMemoryHooker.Rehook();
		return st;
	}
	if(reply==PEDOLL_EXECUTE_REJECT)
	{
		return 0;
	}
	if (reply==PEDOLL_EXECUTE_TERMINATE)
		exit(0);

	return FALSE;
}

APIHooker RegOpenKeyExAHooker;
LSTATUS	APIENTRY hook_RegOpenKeyExA(
	__in HKEY hKey,
	__in_opt LPCSTR lpSubKey,
	__in_opt DWORD ulOptions,
	__in REGSAM samDesired,
	__out PHKEY phkResult
)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	
	if (hKey == HKEY_CLASSES_ROOT)
	{
		sprintf(HookMessage, "%s <%s> <%s>",PEDOLL_QUERY_REGOPENKEYEXA, "HKEY_CLASSES_ROOT", lpSubKey);
	}
	if (hKey == HKEY_CURRENT_CONFIG)
	{
		sprintf(HookMessage, "%s <%s> <%s>",PEDOLL_QUERY_REGOPENKEYEXA, "HKEY_CURRENT_CONFIG", lpSubKey);
	}
	if (hKey == HKEY_CURRENT_USER)
	{
		sprintf(HookMessage, "%s <%s> <%s>",PEDOLL_QUERY_REGOPENKEYEXA, "HKEY_CURRENT_USER", lpSubKey);
	}
	if (hKey == HKEY_LOCAL_MACHINE)
	{
		sprintf(HookMessage, "%s <%s> <%s>",PEDOLL_QUERY_REGOPENKEYEXA, "HKEY_LOCAL_MACHINE", lpSubKey);
	}
	if (hKey == HKEY_USERS)
	{
		sprintf(HookMessage, "%s <%s> <%s>",PEDOLL_QUERY_REGOPENKEYEXA, "HKEY_USERS", lpSubKey);
	}
	int reply = MonitorQuery(HookMessage,dwesp);

	if (reply == PEDOLL_EXECUTE_PASS)
	{
		RegOpenKeyExAHooker.Unhook();
		LSTATUS st= RegOpenKeyExA(hKey, lpSubKey, ulOptions, samDesired, phkResult);
		RegOpenKeyExAHooker.Rehook();
		return st;
	}
	if(reply==PEDOLL_EXECUTE_REJECT)
	{
		return 0;
	}
	if (reply==PEDOLL_EXECUTE_TERMINATE)
		exit(0);
	return 0;
}

APIHooker RegOpenKeyExWHooker;
LSTATUS	APIENTRY hook_RegOpenKeyExW(
	__in HKEY hKey,
	__in_opt LPCWSTR lpSubKey,
	__in_opt DWORD ulOptions,
	__in REGSAM samDesired,
	__out PHKEY phkResult
	)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	if (hKey == HKEY_CLASSES_ROOT)
	{
		sprintf(HookMessage, "%s <%s> <%s>",PEDOLL_QUERY_REGOPENKEYEXW, "HKEY_CLASSES_ROOT", UnicodeToANSI(lpSubKey).c_str());
	}
	if (hKey == HKEY_CURRENT_CONFIG)
	{
		sprintf(HookMessage, "%s <%s> <%s>",PEDOLL_QUERY_REGOPENKEYEXW, "HKEY_CURRENT_CONFIG", UnicodeToANSI(lpSubKey).c_str());
	}
	if (hKey == HKEY_CURRENT_USER)
	{
		sprintf(HookMessage, "%s <%s> <%s>",PEDOLL_QUERY_REGOPENKEYEXW, "HKEY_CURRENT_USER", UnicodeToANSI(lpSubKey).c_str());
	}
	if (hKey == HKEY_LOCAL_MACHINE)
	{
		sprintf(HookMessage, "%s <%s> <%s>",PEDOLL_QUERY_REGOPENKEYEXW, "HKEY_LOCAL_MACHINE", UnicodeToANSI(lpSubKey).c_str());
	}
	if (hKey == HKEY_USERS)
	{
		sprintf(HookMessage, "%s <%s> <%s>",PEDOLL_QUERY_REGOPENKEYEXW, "HKEY_USERS", UnicodeToANSI(lpSubKey).c_str());
	}
	int reply = MonitorQuery(HookMessage,dwesp);

	if (reply == PEDOLL_EXECUTE_PASS)
	{
		RegOpenKeyExWHooker.Unhook();
		LSTATUS st= RegOpenKeyExW(hKey, lpSubKey, ulOptions, samDesired, phkResult);
		RegOpenKeyExWHooker.Rehook();
		return st;
	}
	if(reply==PEDOLL_EXECUTE_REJECT)
	{
		return 0;
	}
	if (reply==PEDOLL_EXECUTE_TERMINATE)
		exit(0);
	return 0;
}


APIHooker RegSetKeyValueAHooker;
LSTATUS	APIENTRY hook_RegSetKeyValueA(
	__in        HKEY     hKey,
	__in_opt    LPCSTR  lpSubKey,
	__in_opt    LPCSTR  lpValueName,
	__in        DWORD    dwType,
	__in_bcount_opt(cbData) LPCVOID  lpData,
	__in        DWORD    cbData
	)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	if (hKey == HKEY_CLASSES_ROOT)
	{
		if(dwType==REG_DWORD)
		sprintf(HookMessage, "%s <%s> <%s\\%s> <0x%08X>",PEDOLL_QUERY_REGSETKEYVALUEA, "HKEY_CLASSES_ROOT", lpSubKey,lpValueName,*(DWORD *)lpData);
		else if(dwType==REG_SZ)
		sprintf(HookMessage, "%s <%s> <%s\\%s> <%s>",PEDOLL_QUERY_REGSETKEYVALUEA, "HKEY_CLASSES_ROOT", lpSubKey,lpValueName,lpData);
		else
		sprintf(HookMessage, "%s <%s> <%s\\%s> <%s>",PEDOLL_QUERY_REGSETKEYVALUEA, "HKEY_CLASSES_ROOT", lpSubKey,lpValueName,"SZ_OTHERS");
	}
	if (hKey == HKEY_CURRENT_CONFIG)
	{
		if(dwType==REG_DWORD)
			sprintf(HookMessage, "%s <%s> <%s\\%s> <0x%08X>",PEDOLL_QUERY_REGSETKEYVALUEA, "HKEY_CURRENT_CONFIG", lpSubKey,lpValueName,*(DWORD *)lpData);
		else if(dwType==REG_SZ)
			sprintf(HookMessage, "%s <%s> <%s\\%s> <%s>",PEDOLL_QUERY_REGSETKEYVALUEA, "HKEY_CURRENT_CONFIG", lpSubKey,lpValueName,lpData);
		else
			sprintf(HookMessage, "%s <%s> <%s\\%s> <%s>",PEDOLL_QUERY_REGSETKEYVALUEA, "HKEY_CURRENT_CONFIG", lpSubKey,lpValueName,"SZ_OTHERS");

	}
	if (hKey == HKEY_CURRENT_USER)
	{
		if(dwType==REG_DWORD)
			sprintf(HookMessage, "%s <%s> <%s\\%s> <0x%08X>",PEDOLL_QUERY_REGSETKEYVALUEA, "HKEY_CURRENT_USER", lpSubKey,lpValueName,*(DWORD *)lpData);
		else if(dwType==REG_SZ)
			sprintf(HookMessage, "%s <%s> <%s\\%s> <%s>",PEDOLL_QUERY_REGSETKEYVALUEA, "HKEY_CURRENT_USER", lpSubKey,lpValueName,lpData);
		else
			sprintf(HookMessage, "%s <%s> <%s\\%s> <%s>",PEDOLL_QUERY_REGSETKEYVALUEA, "HKEY_CURRENT_USER", lpSubKey,lpValueName,"SZ_OTHERS");

	}
	if (hKey == HKEY_LOCAL_MACHINE)
	{
		if(dwType==REG_DWORD)
			sprintf(HookMessage, "%s <%s> <%s\\%s> <0x%08X>",PEDOLL_QUERY_REGSETKEYVALUEA, "HKEY_LOCAL_MACHINE", lpSubKey,lpValueName,*(DWORD *)lpData);
		else if(dwType==REG_SZ)
			sprintf(HookMessage, "%s <%s> <%s\\%s> <%s>",PEDOLL_QUERY_REGSETKEYVALUEA, "HKEY_LOCAL_MACHINE", lpSubKey,lpValueName,lpData);
		else
			sprintf(HookMessage, "%s <%s> <%s\\%s> <%s>",PEDOLL_QUERY_REGSETKEYVALUEA, "HKEY_LOCAL_MACHINE", lpSubKey,lpValueName,"SZ_OTHERS");
	}
	if (hKey == HKEY_USERS)
	{
		if(dwType==REG_DWORD)
			sprintf(HookMessage, "%s <%s> <%s\\%s> <0x%08X>",PEDOLL_QUERY_REGSETKEYVALUEA, "HKEY_USERS", lpSubKey,lpValueName,*(DWORD *)lpData);
		else if(dwType==REG_SZ)
			sprintf(HookMessage, "%s <%s> <%s\\%s> <%s>",PEDOLL_QUERY_REGSETKEYVALUEA, "HKEY_USERS", lpSubKey,lpValueName,lpData);
		else
			sprintf(HookMessage, "%s <%s> <%s\\%s> <%s>",PEDOLL_QUERY_REGSETKEYVALUEA, "HKEY_USERS", lpSubKey,lpValueName,"SZ_OTHERS");
	}
	if (dwType==REG_SZ)
	{
		sprintf(HookMessage,"%s <%s>",HookMessage,lpData);
	}
	else if (dwType==REG_DWORD)
	{
		sprintf(HookMessage,"%s <0x%x>",HookMessage,*((DWORD *)lpData));
	}
	else
	{
		sprintf(HookMessage,"%s <others>",HookMessage);
	}
	int reply=MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		RegSetKeyValueAHooker.Unhook();
		LSTATUS st= RegSetKeyValueA(hKey, lpSubKey, lpValueName, dwType, lpData, cbData);
		RegSetKeyValueAHooker.Rehook();
		return st;
	}
		
	if(reply==PEDOLL_EXECUTE_REJECT)
	{
		return 0;
	}
	if(reply==PEDOLL_EXECUTE_TERMINATE)
	exit(0);
	return 0;
}

APIHooker RegQueryValueExAHooker;
LONG WINAPI hook_RegQueryValueExA(
	__in HKEY hKey,
	__in_opt LPCSTR lpValueName,
	__reserved LPDWORD lpReserved,
	__out_opt LPDWORD lpType,
	__out_opt LPBYTE lpData,
	__inout_opt LPDWORD lpcbData
	)
{
	RegQueryValueExAHooker.Unhook();
	LSTATUS st= RegQueryValueExA(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
	RegQueryValueExAHooker.Rehook();
	if (st!=ERROR_SUCCESS||lpData==NULL)
	{
		return st;
	}
		
	if(*lpType==REG_DWORD)
			sprintf(HookMessage, "%s\\%s\\%s\\ 0x%08X","Registry query-->", "LastKey=", lpValueName,*(DWORD *)lpData);
		else if(*lpType==REG_SZ)
			sprintf(HookMessage, "%s\\%s\\%s\\ %s","Registry query-->", "LastKey=", lpValueName,lpData);
		else
			sprintf(HookMessage, "%s\\%s\\%s\\ %s","Registry query-->", "LastKey=", lpValueName,"SZ_OTHERS");
	

	g_PeDollNetwork.SendControllerMessage(HookMessage);
	return st;
}

APIHooker RegQueryValueExWHooker;
LONG WINAPI hook_RegQueryValueExW(
	__in HKEY hKey,
	__in_opt LPCWSTR lpValueName,
	__reserved LPDWORD lpReserved,
	__out_opt LPDWORD lpType,
	LPBYTE lpData,
	__inout_opt LPDWORD lpcbData
	)
{
	RegQueryValueExWHooker.Unhook();
	LSTATUS st= RegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
	RegQueryValueExWHooker.Rehook();

	if (hKey == HKEY_CLASSES_ROOT)
	{
		if(*lpType==REG_DWORD)
			sprintf(HookMessage, "%s\\%s\\%s\\ 0x%08X","Registry query-->", "HKEY_CLASSES_ROOT",  UnicodeToANSI(lpValueName),*(DWORD *)lpData);
		else if(*lpType==REG_SZ)
			sprintf(HookMessage, "%s\\%s\\%s\\ %s","Registry query-->", "HKEY_CLASSES_ROOT",  UnicodeToANSI(lpValueName),lpData);
		else
			sprintf(HookMessage, "%s\\%s\\%s\\ %s","Registry query-->", "HKEY_CLASSES_ROOT",  UnicodeToANSI(lpValueName),"SZ_OTHERS");
	}
	if (hKey == HKEY_CURRENT_CONFIG)
	{
		if(*lpType==REG_DWORD)
			sprintf(HookMessage, "%s\\%s\\%s\\ 0x%08X","Registry query-->", "HKEY_CURRENT_CONFIG",  UnicodeToANSI(lpValueName),*(DWORD *)lpData);
		else if(*lpType==REG_SZ)
			sprintf(HookMessage, "%s\\%s\\%s\\ %s","Registry query-->", "HKEY_CURRENT_CONFIG",  UnicodeToANSI(lpValueName),lpData);
		else
			sprintf(HookMessage, "%s\\%s\\%s\\ %s","Registry query-->", "HKEY_CURRENT_CONFIG",  UnicodeToANSI(lpValueName),"SZ_OTHERS");

	}
	if (hKey == HKEY_CURRENT_USER)
	{
		if(*lpType==REG_DWORD)
			sprintf(HookMessage, "%s\\%s\\%s\\ 0x%08X","Registry query-->", "HKEY_CURRENT_USER",  UnicodeToANSI(lpValueName),*(DWORD *)lpData);
		else if(*lpType==REG_SZ)
			sprintf(HookMessage, "%s\\%s\\%s\\ %s","Registry query-->", "HKEY_CURRENT_USER",  UnicodeToANSI(lpValueName),lpData);
		else
			sprintf(HookMessage, "%s\\%s\\%s\\ %s","Registry query-->", "HKEY_CURRENT_USER",  UnicodeToANSI(lpValueName),"SZ_OTHERS");

	}
	if (hKey == HKEY_LOCAL_MACHINE)
	{
		if(*lpType==REG_DWORD)
			sprintf(HookMessage, "%s\\%s\\%s\\ 0x%08X","Registry query-->", "HKEY_LOCAL_MACHINE",  UnicodeToANSI(lpValueName),*(DWORD *)lpData);
		else if(*lpType==REG_SZ)
			sprintf(HookMessage, "%s\\%s\\%s\\ %s","Registry query-->", "HKEY_LOCAL_MACHINE",  UnicodeToANSI(lpValueName),lpData);
		else
			sprintf(HookMessage, "%s\\%s\\%s\\ %s","Registry query-->", "HKEY_LOCAL_MACHINE",  UnicodeToANSI(lpValueName),"SZ_OTHERS");
	}
	if (hKey == HKEY_USERS)
	{
		if(*lpType==REG_DWORD)
			sprintf(HookMessage, "%s\\%s\\%s\\ 0x%08X","Registry query-->", "HKEY_USERS",  UnicodeToANSI(lpValueName),*(DWORD *)lpData);
		else if(*lpType==REG_SZ)
			sprintf(HookMessage, "%s\\%s\\%s\\ %s","Registry query-->", "HKEY_USERS",  UnicodeToANSI(lpValueName),lpData);
		else
			sprintf(HookMessage, "%s\\%s\\%s\\ %s","Registry query-->", "HKEY_USERS",  UnicodeToANSI(lpValueName),"SZ_OTHERS");
	}
	if (*lpType==REG_SZ)
	{
		sprintf(HookMessage,"%s <%s>",HookMessage,lpData);
	}

	g_PeDollNetwork.SendControllerMessage(HookMessage);
	return st;
}

APIHooker RegSetValueExWHooker;
LSTATUS
	APIENTRY
	hook_RegSetValueExW (
	__in HKEY hKey,
	__in_opt LPCWSTR lpValueName,
	__reserved DWORD Reserved,
	__in DWORD dwType,
	__in_bcount_opt(cbData) CONST BYTE* lpData,
	__in DWORD cbData
	)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	if (hKey == HKEY_CLASSES_ROOT)
	{
		if(dwType==REG_DWORD)
			sprintf(HookMessage, "%s <%s> <%s> <0x%08X>",PEDOLL_QUERY_REGSETVALUEEXW, "HKEY_CLASSES_ROOT",  UnicodeToANSI(lpValueName).c_str(),*(DWORD *)lpData);
		else if(dwType==REG_SZ)
			sprintf(HookMessage, "%s <%s> <%s> <%s>",PEDOLL_QUERY_REGSETVALUEEXW, "HKEY_CLASSES_ROOT",  UnicodeToANSI(lpValueName).c_str(),lpData);
		else
			sprintf(HookMessage, "%s <%s> <%s> <%s>",PEDOLL_QUERY_REGSETVALUEEXW, "HKEY_CLASSES_ROOT",  UnicodeToANSI(lpValueName).c_str(),"SZ_OTHERS");

	}
	if (hKey == HKEY_CURRENT_CONFIG)
	{

		if(dwType==REG_DWORD)
			sprintf(HookMessage, "%s <%s> <%s> <0x%08X>",PEDOLL_QUERY_REGSETVALUEEXW, "HKEY_CLASSES_ROOT",  UnicodeToANSI(lpValueName).c_str(),*(DWORD *)lpData);
		else if(dwType==REG_SZ)
			sprintf(HookMessage, "%s <%s> <%s> <%s>",PEDOLL_QUERY_REGSETVALUEEXW, "HKEY_CLASSES_ROOT",  UnicodeToANSI(lpValueName).c_str(),lpData);
		else
			sprintf(HookMessage, "%s <%s> <%s> <%s>",PEDOLL_QUERY_REGSETVALUEEXW, "HKEY_CLASSES_ROOT",  UnicodeToANSI(lpValueName).c_str(),"SZ_OTHERS");

	}
	if (hKey == HKEY_CURRENT_USER)
	{
		if(dwType==REG_DWORD)
			sprintf(HookMessage, "%s <%s> <%s> <0x%08X>",PEDOLL_QUERY_REGSETVALUEEXW, "HKEY_CURRENT_USER",  UnicodeToANSI(lpValueName).c_str(),*(DWORD *)lpData);
		else if(dwType==REG_SZ)
			sprintf(HookMessage, "%s <%s> <%s> <%s>",PEDOLL_QUERY_REGSETVALUEEXW, "HKEY_CURRENT_USER",  UnicodeToANSI(lpValueName).c_str(),lpData);
		else
			sprintf(HookMessage, "%s <%s> <%s> <%s>",PEDOLL_QUERY_REGSETVALUEEXW, "HKEY_CURRENT_USER",  UnicodeToANSI(lpValueName).c_str(),"SZ_OTHERS");
	}

	if (hKey == HKEY_LOCAL_MACHINE)
	{
		if(dwType==REG_DWORD)
			sprintf(HookMessage, "%s <%s> <%s> <0x%08X>",PEDOLL_QUERY_REGSETVALUEEXW, "HKEY_LOCAL_MACHINE",  UnicodeToANSI(lpValueName).c_str(),*(DWORD *)lpData);
		else if(dwType==REG_SZ)
			sprintf(HookMessage, "%s <%s> <%s> <%s>",PEDOLL_QUERY_REGSETVALUEEXW, "HKEY_LOCAL_MACHINE",  UnicodeToANSI(lpValueName).c_str(),lpData);
		else
			sprintf(HookMessage, "%s <%s> <%s> <%s>",PEDOLL_QUERY_REGSETVALUEEXW, "HKEY_LOCAL_MACHINE",  UnicodeToANSI(lpValueName).c_str(),"SZ_OTHERS");
	}
	if (hKey == HKEY_USERS)
	{
		if(dwType==REG_DWORD)
			sprintf(HookMessage, "%s <%s> <%s> <0x%08X>",PEDOLL_QUERY_REGSETVALUEEXW, "HKEY_USERS",  UnicodeToANSI(lpValueName).c_str(),*(DWORD *)lpData);
		else if(dwType==REG_SZ)
			sprintf(HookMessage, "%s <%s> <%s> <%s>",PEDOLL_QUERY_REGSETVALUEEXW, "HKEY_USERS",  UnicodeToANSI(lpValueName).c_str(),lpData);
		else
			sprintf(HookMessage, "%s <%s> <%s> <%s>",PEDOLL_QUERY_REGSETVALUEEXW, "HKEY_USERS",  UnicodeToANSI(lpValueName).c_str(),"SZ_OTHERS");
	}
	if (dwType==REG_SZ)
	{
		sprintf(HookMessage,"%s <%s>",HookMessage,lpData);
	}
	else if (dwType==REG_DWORD)
	{
		sprintf(HookMessage,"%s <0x%x>",HookMessage,*((DWORD *)lpData));
	}
	else
	{
		sprintf(HookMessage,"%s <others>",HookMessage);
	}
	int reply = MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		RegSetValueExWHooker.Unhook();
		LSTATUS st = RegSetValueExW(hKey,lpValueName,Reserved,dwType,lpData,cbData);
		RegSetValueExWHooker.Rehook();
		return st;
	}

	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return 0;
	}
	if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);
	return 0;
}

APIHooker RegSetValueExAHooker;
LSTATUS	APIENTRY hook_RegSetValueExA(
	__in HKEY hKey,
	__in_opt LPCSTR lpValueName,
	__reserved DWORD Reserved,
	__in DWORD dwType,
	__in_bcount_opt(cbData) CONST BYTE* lpData,
	__in DWORD cbData
)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	
	sprintf(HookMessage,"%s <%s> <%s>",PEDOLL_QUERY_REGSETVALUEEXA, "LastKey=",  lpValueName);
	

	if (dwType==REG_SZ)
	{
		sprintf(HookMessage,"%s <%s>",HookMessage,lpData);
	}
	else if (dwType==REG_DWORD)
	{
		sprintf(HookMessage,"%s <0x%x>",HookMessage,*((DWORD *)lpData));
	}
	else
	{
		sprintf(HookMessage,"%s <others>",HookMessage);
	}
	int reply = MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		RegSetValueExAHooker.Unhook();
		LSTATUS st = RegSetValueExA(hKey,lpValueName,Reserved,dwType,lpData,cbData);
		RegSetValueExAHooker.Rehook();
		return st;
	}

	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return 0;
	}
	if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);
	return 0;
}

APIHooker RegOpenKeyAHooker;
LSTATUS WINAPI	hook_RegOpenKeyA (
	__in HKEY hKey,
	__in_opt LPCSTR lpSubKey,
	__out PHKEY phkResult
	)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	if(hKey==HKEY_CLASSES_ROOT)
	{
		sprintf(HookMessage,"%s <%s> <%s>",PEDOLL_QUERY_REGOPENKEYA,"HKEY_CLASSES_ROOT",lpSubKey);
	}
	if(hKey==HKEY_CURRENT_CONFIG)
	{
		sprintf(HookMessage,"%s <%s> <%s>",PEDOLL_QUERY_REGOPENKEYA,"HKEY_CURRENT_CONFIG",lpSubKey);
	}
	if(hKey==HKEY_CURRENT_USER)
	{
		sprintf(HookMessage,"%s <%s> <%s>",PEDOLL_QUERY_REGOPENKEYA,"HKEY_CURRENT_USER",lpSubKey);
	}
	if(hKey==HKEY_LOCAL_MACHINE)
	{
		sprintf(HookMessage,"%s <%s> <%s>",PEDOLL_QUERY_REGOPENKEYA,"HKEY_LOCAL_MACHINE",lpSubKey);
	}
	if(hKey==HKEY_USERS)
	{
		sprintf(HookMessage,"%s <%s> <%s>",PEDOLL_QUERY_REGOPENKEYA,"HKEY_USERS",lpSubKey);
	}
	int reply=MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		RegOpenKeyAHooker.Unhook();
		LSTATUS st = RegOpenKeyA(hKey, lpSubKey, phkResult);
		RegOpenKeyAHooker.Rehook();
		return st;
	}
	if(reply==PEDOLL_EXECUTE_REJECT)
	{
		return 0;
	}
	if (reply==PEDOLL_EXECUTE_TERMINATE)
	exit(0);
	return 0;
}


APIHooker RegCreateKeyAHooker;
LSTATUS WINAPI hook_RegCreateKeyA( __in HKEY hKey, __in_opt LPCSTR lpSubKey, __out PHKEY phkResult )
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	if(hKey==HKEY_CLASSES_ROOT)
	{
		sprintf(HookMessage,"%s <%s> <%s>",PEDOLL_QUERY_REGCREATEKEYA,"HKEY_CLASSES_ROOT",lpSubKey);
	}
	if(hKey==HKEY_CURRENT_CONFIG)
	{
		sprintf(HookMessage,"%s <%s> <%s>",PEDOLL_QUERY_REGCREATEKEYA,"HKEY_CURRENT_CONFIG",lpSubKey);
	}
	if(hKey==HKEY_CURRENT_USER)
	{
		sprintf(HookMessage,"%s <%s> <%s>",PEDOLL_QUERY_REGCREATEKEYA,"HKEY_CURRENT_USER",lpSubKey);
	}
	if(hKey==HKEY_LOCAL_MACHINE)
	{
		sprintf(HookMessage,"%s <%s> <%s>",PEDOLL_QUERY_REGCREATEKEYA,"HKEY_LOCAL_MACHINE",lpSubKey);
	}
	if(hKey==HKEY_USERS)
	{
		sprintf(HookMessage,"%s <%s> <%s>",PEDOLL_QUERY_REGCREATEKEYA,"HKEY_USERS",lpSubKey);
	}
	int reply=MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		RegCreateKeyAHooker.Unhook();
		LSTATUS st = RegCreateKeyA(hKey, lpSubKey, phkResult);
		RegCreateKeyAHooker.Rehook();
		return st;
	}
	if(reply==PEDOLL_EXECUTE_REJECT)
	{
		return 0;
	}
	if (reply==PEDOLL_EXECUTE_TERMINATE)
		exit(0);

	return 0;
}


APIHooker FindWindowExAHooker;
HWND WINAPI
	hook_FindWindowExA(
	__in_opt HWND hWndParent,
	__in_opt HWND hWndChildAfter,
	__in_opt LPCSTR lpszClass,
	__in_opt LPCSTR lpszWindow)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	sprintf(HookMessage, "%s <%s> <%s>",PEDOLL_QUERY_FINDWINDOWEXA,lpszClass,lpszWindow);
	int reply=MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		FindWindowExAHooker.Unhook();
		HWND st = FindWindowExA(hWndParent, hWndChildAfter, lpszClass,lpszWindow);
		FindWindowExAHooker.Rehook();
		return st;
	}
	if(reply==PEDOLL_EXECUTE_REJECT)
	{
		return 0;
	}
	if (reply==PEDOLL_EXECUTE_TERMINATE)
		exit(0);

	return 0;
}

APIHooker FindWindowExWHooker;
HWND WINAPI
	hook_FindWindowExW(
	__in_opt HWND hWndParent,
	__in_opt HWND hWndChildAfter,
	__in_opt LPCWSTR lpszClass,
	__in_opt LPCWSTR lpszWindow)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	wsprintfW((LPWSTR)HookMessage, L"%s <%s> <%s>",lpszClass,lpszWindow);
	int reply=MonitorQuery((char *)UnicodeToANSI((LPCWSTR)HookMessage).c_str(),dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		FindWindowExWHooker.Unhook();
		HWND st = FindWindowExW(hWndParent, hWndChildAfter, lpszClass,lpszWindow);
		FindWindowExWHooker.Rehook();
		return st;
	}
	if(reply==PEDOLL_EXECUTE_REJECT)
	{
		return 0;
	}
	if (reply==PEDOLL_EXECUTE_TERMINATE)
		exit(0);

	return 0;
}

APIHooker RegCreateKeyExWHooker;
LSTATUS
	APIENTRY
	hook_RegCreateKeyExW (
	__in HKEY hKey,
	__in LPCWSTR lpSubKey,
	__reserved DWORD Reserved,
	__in_opt LPWSTR lpClass,
	__in DWORD dwOptions,
	__in REGSAM samDesired,
	__in_opt CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	__out PHKEY phkResult,
	__out_opt LPDWORD lpdwDisposition
	)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	if(hKey==HKEY_CLASSES_ROOT)
	{
		sprintf(HookMessage,"%s <%s> <%s>",PEDOLL_QUERY_REGCREATEKEYEXW,"HKEY_CLASSES_ROOT",UnicodeToANSI(lpSubKey).c_str());
	}
	if(hKey==HKEY_CURRENT_CONFIG)
	{
		sprintf(HookMessage,"%s <%s> <%s>",PEDOLL_QUERY_REGCREATEKEYEXW,"HKEY_CURRENT_CONFIG",UnicodeToANSI(lpSubKey).c_str());
	}
	if(hKey==HKEY_CURRENT_USER)
	{
		sprintf(HookMessage,"%s <%s> <%s>",PEDOLL_QUERY_REGCREATEKEYEXW,"HKEY_CURRENT_USER",UnicodeToANSI(lpSubKey).c_str());
	}
	if(hKey==HKEY_LOCAL_MACHINE)
	{
		sprintf(HookMessage,"%s <%s> <%s>",PEDOLL_QUERY_REGCREATEKEYEXW,"HKEY_LOCAL_MACHINE",UnicodeToANSI(lpSubKey).c_str());
	}
	if(hKey==HKEY_USERS)
	{
		sprintf(HookMessage,"%s <%s> <%s>",PEDOLL_QUERY_REGCREATEKEYEXW,"HKEY_USERS",UnicodeToANSI(lpSubKey).c_str());
	}
	int reply=MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		RegCreateKeyExWHooker.Unhook();
		LSTATUS st = RegCreateKeyExW(hKey, lpSubKey,Reserved,lpClass,dwOptions,samDesired,lpSecurityAttributes,phkResult,lpdwDisposition);
		RegCreateKeyExWHooker.Rehook();
		return st;
	}
	if(reply==PEDOLL_EXECUTE_REJECT)
	{
		return 0;
	}
	if (reply==PEDOLL_EXECUTE_TERMINATE)
		exit(0);
	return 0;
}

APIHooker RegCreateKeyExAHooker;
LSTATUS WINAPI hook_RegCreateKeyExA( __in HKEY hKey,
	__in LPCSTR lpSubKey,
	__reserved DWORD Reserved,
	__in_opt LPSTR lpClass,
	__in DWORD dwOptions,
	__in REGSAM samDesired,
	__in_opt CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	__out PHKEY phkResult,
	__out_opt LPDWORD lpdwDisposition )
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	if(hKey==HKEY_CLASSES_ROOT)
	{
		sprintf(HookMessage,"%s <%s> <%s>",PEDOLL_QUERY_REGCREATEKEYEXA,"HKEY_CLASSES_ROOT",lpSubKey);
	}
	if(hKey==HKEY_CURRENT_CONFIG)
	{
		sprintf(HookMessage,"%s <%s> <%s>",PEDOLL_QUERY_REGCREATEKEYEXA,"HKEY_CURRENT_CONFIG",lpSubKey);
	}
	if(hKey==HKEY_CURRENT_USER)
	{
		sprintf(HookMessage,"%s <%s> <%s>",PEDOLL_QUERY_REGCREATEKEYEXA,"HKEY_CURRENT_USER",lpSubKey);
	}
	if(hKey==HKEY_LOCAL_MACHINE)
	{
		sprintf(HookMessage,"%s <%s> <%s>",PEDOLL_QUERY_REGCREATEKEYEXA,"HKEY_LOCAL_MACHINE",lpSubKey);
	}
	if(hKey==HKEY_USERS)
	{
		sprintf(HookMessage,"%s <%s> <%s>",PEDOLL_QUERY_REGCREATEKEYEXA,"HKEY_USERS",lpSubKey);
	}
	int reply=MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		RegCreateKeyExAHooker.Unhook();
		LSTATUS st = RegCreateKeyExA(hKey, lpSubKey,Reserved,lpClass,dwOptions,samDesired,lpSecurityAttributes,phkResult,lpdwDisposition);
		RegCreateKeyExAHooker.Rehook();
		return st;
	}
	if(reply==PEDOLL_EXECUTE_REJECT)
	{
		return 0;
	}
	if (reply==PEDOLL_EXECUTE_TERMINATE)
		exit(0);
	return 0;
}


APIHooker CreateFileAHooker;
HANDLE	WINAPI	hook_CreateFileA(
	__in     LPCSTR lpFileName,
	__in     DWORD dwDesiredAccess,
	__in     DWORD dwShareMode,
	__in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	__in     DWORD dwCreationDisposition,
	__in     DWORD dwFlagsAndAttributes,
	__in_opt HANDLE hTemplateFile
)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	sprintf(HookMessage, "%s <%s> <%s>",PEDOLL_QUERY_CREATEFILEA,"UNKNOW",lpFileName);
	switch(dwCreationDisposition)
	{
	case CREATE_NEW:
	case CREATE_ALWAYS:
		{
			sprintf(HookMessage,  "%s <%s> <%s>",PEDOLL_QUERY_CREATEFILEA,"CREATE",lpFileName);
		}
		break;
	case OPEN_EXISTING:
		{
			sprintf(HookMessage,  "CREATEFILEA <%s> <%s>","OPEN", lpFileName);
		}
		break;
	case OPEN_ALWAYS:
		{
			if(GetFileAttributes(lpFileName)!=INVALID_FILE_ATTRIBUTES)
			sprintf(HookMessage,  "CREATEFILEA <%s> <%s>","OPEN", lpFileName);
			else
			sprintf(HookMessage,  "CREATEFILEA <%s> <%s>","CREATE", lpFileName);
		}
		break;
	case TRUNCATE_EXISTING:
		{
			sprintf(HookMessage, "CREATEFILEA <%s> <%s>","RESET", lpFileName);
		}
		break;
	}
	
	int reply = MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		CreateFileAHooker.Unhook();
		HANDLE hd = CreateFileA(
			lpFileName,
			dwDesiredAccess,
			dwShareMode,
			lpSecurityAttributes,
			dwCreationDisposition,
			dwFlagsAndAttributes,
			hTemplateFile
		);
		CreateFileAHooker.Rehook();
		return hd;
	}
	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return INVALID_HANDLE_VALUE;
	}
		if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);

	return INVALID_HANDLE_VALUE;

}


APIHooker CreateFileWHooker;
HANDLE
	WINAPI
	hook_CreateFileW(
	__in     LPCWSTR lpFileName,
	__in     DWORD dwDesiredAccess,
	__in     DWORD dwShareMode,
	__in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	__in     DWORD dwCreationDisposition,
	__in     DWORD dwFlagsAndAttributes,
	__in_opt HANDLE hTemplateFile
	)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	sprintf(HookMessage,"%s <%s> <%s>",PEDOLL_QUERY_CREATEFILEW,"UNKNOW",UnicodeToANSI(lpFileName).c_str());
	switch(dwCreationDisposition)
	{
	case CREATE_NEW:
	case CREATE_ALWAYS:
		{
			sprintf(HookMessage, "%s <%s> <%s>",PEDOLL_QUERY_CREATEFILEW,"CREATE", UnicodeToANSI(lpFileName).c_str());
		}
		break;
	case OPEN_EXISTING:
	case OPEN_ALWAYS:
		{
			if(GetFileAttributesW(lpFileName)!=INVALID_FILE_ATTRIBUTES)
				sprintf(HookMessage, "%s <%s> <%s>",PEDOLL_QUERY_CREATEFILEW,"OPEN", UnicodeToANSI(lpFileName).c_str());
			else
				sprintf(HookMessage, "%s <%s> <%s>",PEDOLL_QUERY_CREATEFILEW,"CREATE", UnicodeToANSI(lpFileName).c_str());
		}
		break;
	case TRUNCATE_EXISTING:
		{
			sprintf(HookMessage, "%s <%s> <%s>",PEDOLL_QUERY_CREATEFILEW,"RESET", UnicodeToANSI(lpFileName).c_str());
		}
		break;
	}

	int reply = MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		CreateFileWHooker.Unhook();
		HANDLE hd = CreateFileW(
			lpFileName,
			dwDesiredAccess,
			dwShareMode,
			lpSecurityAttributes,
			dwCreationDisposition,
			dwFlagsAndAttributes,
			hTemplateFile
			);
		CreateFileWHooker.Rehook();

		return hd;
	}
	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return INVALID_HANDLE_VALUE;
	}
	if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);

	return INVALID_HANDLE_VALUE;
}

APIHooker WriteFileHooker;
BOOL
	WINAPI
	hook_WriteFile(
	__in        HANDLE hFile,
	__in_bcount_opt(nNumberOfBytesToWrite) LPCVOID lpBuffer,
	__in        DWORD nNumberOfBytesToWrite,
	__out_opt   LPDWORD lpNumberOfBytesWritten,
	__inout_opt LPOVERLAPPED lpOverlapped
	)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}

	sprintf(HookMessage, "%s <%08X>",PEDOLL_QUERY_WRITEFILE,(DWORD)hFile);


	g_PeDollNetwork.SendDumpPack(HookMessage,(char *)lpBuffer,nNumberOfBytesToWrite);

	sprintf(HookMessage,"%s  <%d>",PEDOLL_QUERY_WRITEFILE,(DWORD)hFile);
	
	int reply = MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		WriteFileHooker.Unhook();
		BOOL ret=WriteFile(hFile,lpBuffer,nNumberOfBytesToWrite,lpNumberOfBytesWritten,lpOverlapped);
		WriteFileHooker.Rehook();

		return ret;
	}
	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return FALSE;
	}
	if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);

	return FALSE;
}

APIHooker DeviceIoControlHooker;
BOOL
	WINAPI
	hook_DeviceIoControl(
	__in        HANDLE hDevice,
	__in        DWORD dwIoControlCode,
	__in_bcount_opt(nInBufferSize) LPVOID lpInBuffer,
	__in        DWORD nInBufferSize,
	__out_bcount_part_opt(nOutBufferSize, *lpBytesReturned) LPVOID lpOutBuffer,
	__in        DWORD nOutBufferSize,
	__out_opt   LPDWORD lpBytesReturned,
	__inout_opt LPOVERLAPPED lpOverlapped
	)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}

	sprintf(HookMessage, "%s <%08X>",PEDOLL_QUERY_DEVICEIOCONTROL,(DWORD)hDevice);

	g_PeDollNetwork.SendDumpPack(HookMessage,(char *)lpInBuffer,nInBufferSize);

	sprintf(HookMessage,"%s <%d>",PEDOLL_QUERY_DEVICEIOCONTROL,(DWORD)hDevice);

	int reply = MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		DeviceIoControlHooker.Unhook();
		BOOL ret=DeviceIoControl(hDevice,dwIoControlCode,lpInBuffer,nInBufferSize,lpOutBuffer,nOutBufferSize,lpBytesReturned,lpOverlapped);
		DeviceIoControlHooker.Rehook();

		return ret;
	}
	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return FALSE;
	}
	if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);

	return FALSE;
}


APIHooker CreateProcessInternalWHooker;
typedef BOOL (WINAPI	*pfnCreateProcessInternalW)(HANDLE hToken,
	LPCWSTR lpApplicationName,
	LPWSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCWSTR lpCurrentDirectory,
	LPSTARTUPINFOW lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation,
	PHANDLE hNewToken);

BOOL WINAPI hook_CreateProcessInternalW(HANDLE hToken,
	LPCWSTR lpApplicationName,
	LPWSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCWSTR lpCurrentDirectory,
	LPSTARTUPINFOW lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation,
	PHANDLE hNewToken)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	wsprintfW((WCHAR *)HookMessage,L"%s <%s> <%s>",PEDOLL_QUERY_LCREATEPROCESSINTERNALW,lpApplicationName,lpCommandLine);
	int reply = MonitorQuery((char *)UnicodeToANSI((LPCWSTR)HookMessage).c_str(),dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		CreateProcessInternalWHooker.Unhook();
		pfnCreateProcessInternalW pfnCPIW = (pfnCreateProcessInternalW)GetProcAddress(LoadLibrary("Kernel32.dll"), "CreateProcessInternalW");
		BOOL ret = pfnCPIW(hToken, lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation, hNewToken);
		CreateProcessInternalWHooker.Rehook();
		return ret;
	}
	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return FALSE;
	}
	if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);
	return FALSE;
}

APIHooker CreateProcessWHooker;
BOOL
	WINAPI
	hook_CreateProcessW(
	__in_opt    LPCWSTR lpApplicationName,
	__inout_opt LPWSTR lpCommandLine,
	__in_opt    LPSECURITY_ATTRIBUTES lpProcessAttributes,
	__in_opt    LPSECURITY_ATTRIBUTES lpThreadAttributes,
	__in        BOOL bInheritHandles,
	__in        DWORD dwCreationFlags,
	__in_opt    LPVOID lpEnvironment,
	__in_opt    LPCWSTR lpCurrentDirectory,
	__in        LPSTARTUPINFOW lpStartupInfo,
	__out       LPPROCESS_INFORMATION lpProcessInformation
	)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	wsprintfW((WCHAR *)HookMessage,L"%s <%s> <%s>",PEDOLL_QUERY_LCREATEPROCESSW,lpApplicationName,lpCommandLine);
	int reply = MonitorQuery((char *)UnicodeToANSI((LPCWSTR)HookMessage).c_str(),dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		CreateProcessWHooker.Unhook();
		BOOL ret=CreateProcessW(lpApplicationName,lpCommandLine,lpProcessAttributes,lpThreadAttributes,bInheritHandles,dwCreationFlags,lpEnvironment,lpCurrentDirectory,lpStartupInfo,lpProcessInformation);
		CreateProcessWHooker.Rehook();
		return ret;
	}
	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return FALSE;
	}
	if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);
	return FALSE;
}

APIHooker CreateProcessAHooker;
BOOL
	WINAPI
	hook_CreateProcessA(
	__in_opt    LPCSTR lpApplicationName,
	__inout_opt LPSTR lpCommandLine,
	__in_opt    LPSECURITY_ATTRIBUTES lpProcessAttributes,
	__in_opt    LPSECURITY_ATTRIBUTES lpThreadAttributes,
	__in        BOOL bInheritHandles,
	__in        DWORD dwCreationFlags,
	__in_opt    LPVOID lpEnvironment,
	__in_opt    LPCSTR lpCurrentDirectory,
	__in        LPSTARTUPINFOA lpStartupInfo,
	__out       LPPROCESS_INFORMATION lpProcessInformation
	)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	sprintf_s(HookMessage, "%s <%s> <%s>",PEDOLL_QUERY_CREATEPROCESSA, lpApplicationName, lpCommandLine);
	int reply = MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		CreateProcessAHooker.Unhook();
		BOOL ret=CreateProcessA(lpApplicationName,lpCommandLine,lpProcessAttributes,lpThreadAttributes,bInheritHandles,dwCreationFlags,lpEnvironment,lpCurrentDirectory,lpStartupInfo,lpProcessInformation);
		CreateProcessAHooker.Rehook();
		return ret;
	}
	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return FALSE;
	}
	if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);
	return FALSE;
}

APIHooker connectHooker;
int WINAPI hook_connect(
	__in SOCKET s,
	__in_bcount(namelen) const struct sockaddr FAR *name,
	__in int namelen)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	sprintf(HookMessage, "%s <%s> <%d>",
		PEDOLL_QUERY_CONNECT,
		inet_ntoa(((SOCKADDR_IN *)name)->sin_addr),
		ntohs(((SOCKADDR_IN *)name)->sin_port)
	);
	int reply = MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		connectHooker.Unhook();
		int ret = connect(s, name, namelen);
		connectHooker.Rehook();
		return ret;
	}
	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return SOCKET_ERROR;
	}
		if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);
	return SOCKET_ERROR;
}

static int sendIndex = 0;

int WINAPI hook_send(SOCKET s, const char FAR *buf, int len, int flags)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	SOCKADDR_IN addrin;
	if(!GetAddressBySocket(s,addrin))
	g_PeDollNetwork.SendDumpPack("UNKNOW",(char *)buf, len);
	else
	{
		sprintf(HookMessage, "%s <%s> <%d>",PEDOLL_QUERY_SEND,inet_ntoa((addrin).sin_addr), ntohs((addrin).sin_port));
		g_PeDollNetwork.SendDumpPack(HookMessage,(char *)buf,len);
	}

	if(!GetAddressBySocket(s,addrin))
	sprintf(HookMessage, "%s <%s> <%d>",PEDOLL_QUERY_SEND,"0.0.0.0",0);
	else
	sprintf(HookMessage, "%s <%s> <%d>",PEDOLL_QUERY_SEND,inet_ntoa((addrin).sin_addr), ntohs((addrin).sin_port));

	int reply = MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		sendHooker.Unhook();
		int nRet = send(s, buf, len, flags);
		sendHooker.Rehook();
		return nRet;
	}
	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return SOCKET_ERROR;
	}
	if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);
	return SOCKET_ERROR;
}


static int sendtoIndex = 0;
APIHooker sendtoHooker;
int WINAPI hook_sendto(SOCKET s, const char* buf,
	int len, int flags, const struct sockaddr* to, int tolen)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	sprintf(HookMessage, "%s <%s> <%d>",PEDOLL_QUERY_SENDTO, inet_ntoa(((SOCKADDR_IN *)to)->sin_addr), ntohs(((SOCKADDR_IN *)to)->sin_port));
	g_PeDollNetwork.SendDumpPack(HookMessage,(char *)buf, len);
	sprintf(HookMessage, "%s <%s> <%d>",PEDOLL_QUERY_SENDTO, inet_ntoa(((SOCKADDR_IN *)to)->sin_addr), ntohs(((SOCKADDR_IN *)to)->sin_port));
	int reply = MonitorQuery( HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		sendtoHooker.Unhook();
		int nRet = sendto(s, buf, len,flags,to,tolen);
		sendtoHooker.Rehook();
		return nRet;
	}
	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return SOCKET_ERROR;
	}
	if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);
	return SOCKET_ERROR;
}

static int recvIndex = 0;
APIHooker recvHooker;
int WINAPI hook_recv(SOCKET s, char FAR *buf, int len, int flags)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
		recvHooker.Unhook();
		int nRet = recv(s, buf, len, flags);
		recvHooker.Rehook();
	
	
		SOCKADDR_IN addrin;
		if(!GetAddressBySocket(s,addrin))
			g_PeDollNetwork.SendDumpPack("UNKNOW",(char *)buf, len);
		else
		{
			sprintf(HookMessage, "%s <%s> <%d>",PEDOLL_QUERY_RECV,inet_ntoa((addrin).sin_addr), ntohs((addrin).sin_port));
			g_PeDollNetwork.SendDumpPack(HookMessage,(char *)buf,len);
		}

		if(!GetAddressBySocket(s,addrin))
			sprintf(HookMessage, "%s <%s> <%d>",PEDOLL_QUERY_RECV,"0.0.0.0",0);
		else
			sprintf(HookMessage, "%s <%s> <%d>",PEDOLL_QUERY_RECV,inet_ntoa((addrin).sin_addr), ntohs((addrin).sin_port));

		int reply = MonitorQuery(HookMessage,dwesp);
		if (reply == PEDOLL_EXECUTE_PASS)
		{
			return nRet;
		}
		if (reply == PEDOLL_EXECUTE_REJECT)
		{
			memset(buf,0,len);
			return 0;
		}
		if (reply == PEDOLL_EXECUTE_TERMINATE)
			exit(0);

		return 0;

}

static int recvfromIndex = 0;
APIHooker recvfromHooker;
int WINAPI hook_recvfrom(SOCKET s, char* buf, int len, int flags, struct sockaddr* from, int* fromlen)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	recvfromHooker.Unhook();
	int nRet = recvfrom(s, buf, len, flags,from, fromlen);
	recvfromHooker.Rehook();
	
	sprintf(HookMessage,  "%s <%s> <%d>",PEDOLL_QUERY_RECVFROM, inet_ntoa(((SOCKADDR_IN *)from)->sin_addr), ntohs(((SOCKADDR_IN *)from)->sin_port));
	g_PeDollNetwork.SendDumpPack(HookMessage,(char *)buf, len);
	sprintf(HookMessage, "%s <%s> <%d>",PEDOLL_QUERY_RECVFROM, inet_ntoa(((SOCKADDR_IN *)from)->sin_addr), ntohs(((SOCKADDR_IN *)from)->sin_port));
	int reply = MonitorQuery( HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		return nRet;
	}
	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return SOCKET_ERROR;
	}
	if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);
	return SOCKET_ERROR;
}


APIHooker DeleteFileAHooker;
BOOL
	WINAPI
	hook_DeleteFileA(
	__in LPCSTR lpFileName
	)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	sprintf(HookMessage, "%s <%s>",PEDOLL_QUERY_DELETEFILEA, lpFileName);
	int reply = MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		DeleteFileAHooker.Unhook();
		BOOL ret=DeleteFileA(lpFileName);
		DeleteFileAHooker.Rehook();
		return ret;
	}
	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return FALSE;
	}
	if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);
	return FALSE;
}

APIHooker DeleteFileWHooker;
BOOL
	WINAPI
	hook_DeleteFileW(
	__in LPCWSTR lpFileName
	)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	sprintf(HookMessage, "%s <%s>",PEDOLL_QUERY_DELETEFILEW, UnicodeToANSI(lpFileName).c_str());
	int reply = MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		DeleteFileWHooker.Unhook();
		BOOL ret=DeleteFileW(lpFileName);
		DeleteFileWHooker.Rehook();
		return ret;
	}
	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return FALSE;
	}
	if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);
	return FALSE;
}



APIHooker ExitWindowsHooker;
BOOL 
	WINAPI
	hook_ExitWindowsEx(
	__in UINT uFlags,
	__in DWORD dwReason)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	sprintf(HookMessage, PEDOLL_QUERY_EXITWINDOWSEX);
	int reply = MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		ExitWindowsHooker.Unhook();
		BOOL ret=ExitWindowsEx(uFlags,dwReason);
		ExitWindowsHooker.Rehook();
		return ret;
	}
	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return FALSE;
	}
	if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);
	return FALSE;
}

APIHooker TerminateProcessHooker;

BOOL
	WINAPI
	hook_TerminateProcess(
	__in HANDLE hProcess,
	__in UINT uExitCode
	)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	sprintf(HookMessage, "%d <%s>",PEDOLL_QUERY_TERMINATEPROCESS,GetProcessNameByPID((DWORD)hProcess).szExeFile);
	int reply = MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		TerminateProcessHooker.Unhook();
		BOOL ret=TerminateProcess(hProcess,uExitCode);
		TerminateProcessHooker.Rehook();
		return ret;
	}
	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return FALSE;
	}
	if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);
	return FALSE;
}



APIHooker Process32FirstHookerW;
BOOL
	WINAPI
	hook_Process32FirstW(
	HANDLE hSnapshot,
	LPPROCESSENTRY32W lppe
	)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	sprintf(HookMessage, PEDOLL_QUERY_PROCESS32FIRSTW);
	int reply = MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		Process32FirstHookerW.Unhook();
		int nRet = Process32FirstW(hSnapshot,lppe);
		Process32FirstHookerW.Rehook();
		return nRet;
	}
	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return FALSE;
	}
	if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);
	return FALSE;

}


APIHooker Process32NextWHooker;
BOOL
	WINAPI
	hook_Process32NextW(
	HANDLE hSnapshot,
	LPPROCESSENTRY32W lppe
	)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	sprintf(HookMessage, PEDOLL_QUERY_PROCESS32NEXTW);
	int reply = MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		Process32NextWHooker.Unhook();
		int nRet = Process32NextW(hSnapshot,lppe);
		Process32NextWHooker.Rehook();
		return nRet;
	}
	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return FALSE;
	}
	if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);


	return FALSE;
}

APIHooker OpenProcessHooker;
HANDLE
	WINAPI
	hook_OpenProcess(
	__in DWORD dwDesiredAccess,
	__in BOOL bInheritHandle,
	__in DWORD dwProcessId
	)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	sprintf(HookMessage, "%s <%s>",PEDOLL_QUERY_OPENPROCESS,GetProcessNameByPID(dwProcessId).szExeFile);
	int reply = MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		OpenProcessHooker.Unhook();
		HANDLE ret=OpenProcess(dwDesiredAccess,bInheritHandle,dwProcessId);
		OpenProcessHooker.Rehook();
		return ret;
	}
	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return INVALID_HANDLE_VALUE;
	}
	if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);
	return INVALID_HANDLE_VALUE;

}

APIHooker LoadLibraryExWHooker;
HMODULE
	WINAPI
	hook_LoadLibraryExW(
	__in       LPCWSTR lpLibFileName,
	__reserved HANDLE hFile,
	__in       DWORD dwFlags
	)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	sprintf(HookMessage, "%s <%s>",PEDOLL_QUERY_LOADLIBRARYEXW,UnicodeToANSI(lpLibFileName).c_str());
	int reply = MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		LoadLibraryExWHooker.Unhook();
		HMODULE ret=LoadLibraryExW(lpLibFileName,hFile,dwFlags);
		LoadLibraryExWHooker.Rehook();
		return ret;
	}
	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return 0;
	}
	if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);
	return 0;
}

APIHooker CreateWindowExWHooker;
HWND
	WINAPI
	hook_CreateWindowExW(
	__in DWORD dwExStyle,
	__in_opt LPCWSTR lpClassName,
	__in_opt LPCWSTR lpWindowName,
	__in DWORD dwStyle,
	__in int X,
	__in int Y,
	__in int nWidth,
	__in int nHeight,
	__in_opt HWND hWndParent,
	__in_opt HMENU hMenu,
	__in_opt HINSTANCE hInstance,
	__in_opt LPVOID lpParam)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	sprintf(HookMessage, "%s <%s>",PEDOLL_QUERY_CREATEWINDOWEXW,UnicodeToANSI(lpWindowName).c_str());
	int reply = MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		CreateWindowExWHooker.Unhook();
		HWND ret=CreateWindowExW(dwStyle,lpClassName,lpWindowName,dwStyle,X,Y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);
		CreateWindowExWHooker.Rehook();
		return ret;
	}
	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return 0;
	}
	if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);

	return 0;
}
APIHooker SetWindowPosHooker;
BOOL
	WINAPI
	hook_SetWindowPos(
	__in HWND hWnd,
	__in_opt HWND hWndInsertAfter,
	__in int X,
	__in int Y,
	__in int cx,
	__in int cy,
	__in UINT uFlags)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	sprintf(HookMessage, "%s <%d> <%d> <%d> <%d> <%d> <%d>",PEDOLL_QUERY_SETWINDOWPOS,(DWORD)hWnd,(DWORD)hWndInsertAfter,X,Y,cx,cy);
	int reply = MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		SetWindowPosHooker.Unhook();
		BOOL ret=SetWindowPos(hWnd,hWndInsertAfter,X,Y,cx,cy,uFlags);
		SetWindowPosHooker.Rehook();

		return ret;
	}
	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return 0;
	}
	if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);

	return 0;
}


APIHooker SetFileAttributesAHooker;
APIHooker SetFileAttributesWHooker;
BOOL
	WINAPI
	hook_SetFileAttributesA(
	__in LPCSTR lpFileName,
	__in DWORD dwFileAttributes
	)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	sprintf(HookMessage, "%s <%s> <%d>",PEDOLL_QUERY_SETFILEATTRIBUTESA,lpFileName,(DWORD)dwFileAttributes);
	int reply = MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		SetFileAttributesAHooker.Unhook();
		BOOL ret=SetFileAttributesA(lpFileName,dwFileAttributes);
		SetFileAttributesAHooker.Rehook();

		return ret;
	}
	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return FALSE;
	}
	if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);

	return FALSE;
}
BOOL
	WINAPI
	hook_SetFileAttributesW(
	__in LPCWSTR lpFileName,
	__in DWORD dwFileAttributes
	)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	sprintf(HookMessage, "%s <%s> <%d>",PEDOLL_QUERY_SETFILEATTRIBUTESW,UnicodeToANSI(lpFileName).c_str(),(DWORD)dwFileAttributes);
	int reply = MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		SetFileAttributesWHooker.Unhook();
		BOOL ret=SetFileAttributesW(lpFileName,dwFileAttributes);
		SetFileAttributesWHooker.Rehook();

		return ret;
	}
	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return FALSE;
	}
	if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);

	return FALSE;
}


APIHooker SetWindowTextAHooker;
APIHooker SetWindowTextWHooker;
BOOL
	WINAPI
	hook_SetWindowTextA(
	__in HWND hWnd,
	__in_opt LPCSTR lpString)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	dwesp+=0x1c0-64;
	char Text128[128];
	if (strlen(lpString)>=128)
	{
		memcpy(Text128,lpString,127);
		Text128[127]='\0';
	}
	else
	{
		strcpy(Text128,lpString);
	}
	sprintf(HookMessage, "%s <%d> <%s>",PEDOLL_QUERY_SETWINDOWTEXTA,(DWORD)hWnd,Text128);
	int reply = MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		SetWindowTextAHooker.Unhook();
		BOOL ret=SetWindowTextA(hWnd,lpString);
		SetWindowTextAHooker.Rehook();

		return ret;
	}
	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return FALSE;
	}
	if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);

	return FALSE;
}
BOOL
	WINAPI
	hook_SetWindowTextW(
	__in HWND hWnd,
	__in_opt LPCWSTR lpString)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	dwesp+=0x1c0-64;
	char Text128[128];
	string strw=UnicodeToANSI(lpString);
	if (strlen(strw.c_str())>=128)
	{
		memcpy(Text128,strw.c_str(),127);
		Text128[127]='\0';
	}
	else
	{
		strcpy(Text128,strw.c_str());
	}
	sprintf(HookMessage, "%s <%d> <%s>",PEDOLL_QUERY_SETWINDOWTEXTA,(DWORD)hWnd,Text128);
	int reply = MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		SetWindowTextWHooker.Unhook();
		BOOL ret=SetWindowTextW(hWnd,lpString);
		SetWindowTextWHooker.Rehook();

		return ret;
	}
	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return FALSE;
	}
	if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);

	return FALSE;
}
APIHooker MessageBoxAHooker;
APIHooker MessageBoxWHooker;
int	WINAPI
	hook_MessageBoxA(
	__in_opt HWND hWnd,
	__in_opt LPCSTR lpText,
	__in_opt LPCSTR lpCaption,
	__in UINT uType)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	dwesp+=0x1c0;
	char Text128[128];
	char Caption[64];
	if (strlen(lpText)>=128)
	{
		memcpy(Text128,lpText,127);
		Text128[127]='\0';
	}
	else
	{
		strcpy(Text128,lpText);
	}

	if (strlen(lpCaption)>=64)
	{
		memcpy(Caption,lpCaption,63);
		Caption[63]='\0';
	}
	else
	{
		strcpy(Caption,lpCaption);
	}

	sprintf(HookMessage, "%s <%s> <%s>",PEDOLL_QUERY_MESSAGEBOXA,Text128,lpCaption);
	int reply = MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		MessageBoxAHooker.Unhook();
		int ret=MessageBoxA(hWnd,lpText,lpCaption,uType);
		MessageBoxAHooker.Rehook();

		return ret;
	}
	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return IDNO;
	}
	if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);

	return IDOK;
}

int
	WINAPI
	hook_MessageBoxW(
	__in_opt HWND hWnd,
	__in_opt LPCWSTR lpText,
	__in_opt LPCWSTR lpCaption,
	__in UINT uType)
{
	DWORD dwesp;
	_asm
	{
		mov dwesp,esp
	}
	dwesp+=0x1c0;
	char Text128[128];
	char Caption[64];
	string strw=UnicodeToANSI(lpText);
	string strcw=UnicodeToANSI(lpCaption);
	if (strlen(strw.c_str())>=128)
	{
		memcpy(Text128,strw.c_str(),127);
		Text128[127]='\0';
	}
	else
	{
		strcpy(Text128,strw.c_str());
	}

	if (strlen(strcw.c_str())>=64)
	{
		memcpy(Caption,strcw.c_str(),63);
		Text128[63]='\0';
	}
	else
	{
		strcpy(Caption,strcw.c_str());
	}
	sprintf(HookMessage, "%s <%s> <%s>",PEDOLL_QUERY_MESSAGEBOXW,Text128,Caption);
	int reply = MonitorQuery(HookMessage,dwesp);
	if (reply == PEDOLL_EXECUTE_PASS)
	{
		MessageBoxWHooker.Unhook();
		int ret=MessageBoxW(hWnd,lpText,lpCaption,uType);
		MessageBoxWHooker.Rehook();

		return ret;
	}
	if (reply == PEDOLL_EXECUTE_REJECT)
	{
		return IDNO;
	}
	if (reply == PEDOLL_EXECUTE_TERMINATE)
		exit(0);

	return IDOK;
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		if (IsAlreadyRunning())
		{
			exit(0);
		}
	 	memset(HookMessage,0,sizeof(HookMessage));
	 
	 	if(!InitializeNetwork())
	 	{
	 		if(MessageBoxA(NULL,"无法初始化调试通道.","启动信息",MB_YESNO|MB_ICONWARNING)==PEDOLL_EXECUTE_REJECT)
	 			exit(0);
	 	}

	g_PeDollNetwork.WaitForConnect();
	
	g_PeDollNetwork.SendControllerMessage(PEDOLL_VERSION_INFO);

	sprintf(HookMessage,"%s %s",PEDOLL_CONTROLLER_PROCESSNAME_STR,GetProcName());

	g_PeDollNetwork.SendControllerMessage(HookMessage);

	g_PeDollNetwork.SendControllerMessage("Doll is ready for debug,Preload hook configurations & Press \"run\" to startup.");

	g_PeDollNetwork.Preload();

	}
	break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return (TRUE);
}


#define AFD_RECV 0x12017
#define AFD_SEND 0x1201f


typedef struct AFD_WSABUF{
	UINT  len ;
	PCHAR  buf ;
}AFD_WSABUF , *PAFD_WSABUF;

typedef struct AFD_INFO {
	PAFD_WSABUF  BufferArray ; 
	ULONG  BufferCount ; 
	ULONG  AfdFlags ;
	ULONG  TdiFlags ;
} AFD_INFO,  *PAFD_INFO;
typedef LONG NTSTATUS;


const CHAR GetXX[] = "GET ";
const CHAR PostXX[] = "POST ";
const CHAR HttpXX[] = "HTTP";


BOOL LookupSendPacket(PVOID Buffer , ULONG Len)
{
	if (Len < 5)
	{
		return FALSE ; 
	}

	if (memcmp(Buffer , GetXX , 4) == 0 
		||
		memcmp(Buffer , PostXX , 5) == 0 )
	{
		return TRUE ; 
	}
	return FALSE ; 
}  

BOOL LookupRecvPacket(PVOID Buffer , ULONG Len)
{
	if (Len < 4)
	{
		return FALSE ; 
	}

	if (memcmp(Buffer , HttpXX , 4) == 0 )
	{
		return TRUE ; 
	}

	return FALSE ; 
}

typedef NTSTATUS (__stdcall *PROCNtDeviceIoControlFile)(\
	IN HANDLE FileHandle,\
	IN HANDLE Event OPTIONAL,\
	IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,\
	IN PVOID ApcContext OPTIONAL,\
	OUT PIO_STATUS_BLOCK IoStatusBlock,\
	IN ULONG IoControlCode,\
	IN PVOID InputBuffer OPTIONAL,\
	IN ULONG InputBufferLength,\
	OUT PVOID OutputBuffer OPTIONAL,\
	IN ULONG OutputBufferLength\
	);

NTSTATUS __stdcall hook_NtDeviceIoControlFile(
	IN HANDLE FileHandle,
	IN HANDLE Event OPTIONAL,
	IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN ULONG IoControlCode,
	IN PVOID InputBuffer OPTIONAL,
	IN ULONG InputBufferLength,
	OUT PVOID OutputBuffer OPTIONAL,
	IN ULONG OutputBufferLength
	)
{
	
	LONG stat ; 
	NtDeviceIoControlFileHooker.Unhook();
	DWORD pfnntDiow = (DWORD)GetProcAddress(LoadLibrary("ntdll.dll"), "NtDeviceIoControlFile");
	__asm
	{
		push  OutputBufferLength
			push  OutputBuffer
			push  InputBufferLength
			push  InputBuffer 
			push  IoControlCode
			push  IoStatusBlock 
			push  ApcContext
			push  ApcRoutine
			push  Event
			push  FileHandle
			call  pfnntDiow
			mov    stat ,eax
	}
	if (!NT_SUCCESS(stat))
	{
		NtDeviceIoControlFileHooker.Rehook();
		return stat ; 
	}


	if (IoControlCode != AFD_SEND && IoControlCode != AFD_RECV)
	{
		NtDeviceIoControlFileHooker.Rehook();
		return stat ; 
	}

	__try
	{

		PAFD_INFO AfdInfo = (PAFD_INFO)InputBuffer ; 
		PVOID Buffer = AfdInfo->BufferArray->buf ; 
		ULONG Len = AfdInfo->BufferArray->len;

		if (IoControlCode == AFD_SEND)
		{
			if (LookupSendPacket(Buffer , Len))
			{
				g_PeDollNetwork.SendDumpPack("HTTP SEND REQUEST",(char *)Buffer,Len);
			}
		}
		else
		{
			if (LookupRecvPacket(Buffer , Len))
			{
				g_PeDollNetwork.SendDumpPack("HTTP RECV CONTEXT",(char *)Buffer,Len);
			}
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		NtDeviceIoControlFileHooker.Rehook();
		return stat ; 
	}

	NtDeviceIoControlFileHooker.Rehook();
	return stat ; 
}





void HookFunction(char *LibraryName,char *FunctionName,DWORD func,bool bHook,APIHooker *pHooker)
{
	PEDOLL_ACK_PACKET ackpack;
	if (bHook)
	{
		if(pHooker->Hook(LibraryName, FunctionName, (DWORD)func))
		{
			ackpack.Return=1;
		}
		else
		{
			ackpack.Return=0;
		}
	}
	else
	{
		pHooker->Unhook();
		ackpack.Return=1;
	}
	g_PeDollNetwork.SendAckPack((char *)&ackpack,sizeof(PEDOLL_ACK_PACKET));
}




void HookFunctionCommand(char *pCMD,bool bHook)
{
	if (strcmp(pCMD,"DeleteFileA")==0)
	{
		HookFunction("Kernel32.dll", "DeleteFileA", (DWORD)hook_DeleteFileA,bHook,&DeleteFileAHooker);
		return;
	}

	if (strcmp(pCMD,"DeleteFileW")==0)
	{
		HookFunction("Kernel32.dll", "DeleteFileW", (DWORD)hook_DeleteFileW,bHook,&DeleteFileWHooker);
		return;
	}

	if (strcmp(pCMD,"CreateProcessInternalW")==0)
	{
		HookFunction("Kernel32.dll", "CreateProcessInternalW", (DWORD)hook_CreateProcessInternalW,bHook,&CreateProcessInternalWHooker);
		return;
	}

	if (strcmp(pCMD,"CreateProcessW")==0)
	{
		HookFunction("Kernel32.dll", "CreateProcessW", (DWORD)hook_CreateProcessW,bHook,&CreateProcessWHooker);
		return;
	}

	if (strcmp(pCMD,"CreateProcessA")==0)
	{
		HookFunction("Kernel32.dll", "CreateProcessA", (DWORD)hook_CreateProcessA,bHook,&CreateProcessAHooker);
		return;
	}

	if (strcmp(pCMD,"Process32FirstW")==0)
	{
		HookFunction("Kernel32.dll", "Process32FirstW", (DWORD)hook_Process32FirstW,bHook,&Process32FirstHookerW);
		return;
	}

	if (strcmp(pCMD,"Process32NextW")==0)
	{
		HookFunction("Kernel32.dll", "Process32NextW", (DWORD)hook_Process32NextW,bHook,&Process32NextWHooker);
		return;
	}

	if (strcmp(pCMD,"TerminateProcess")==0)
	{
		HookFunction("Kernel32.dll", "TerminateProcess", (DWORD)hook_TerminateProcess,bHook,&TerminateProcessHooker);
		return;
	}

	if (strcmp(pCMD,"OpenProcess")==0)
	{
		HookFunction("Kernel32.dll", "OpenProcess", (DWORD)hook_OpenProcess,bHook,&OpenProcessHooker);
		return;
	}

	if (strcmp(pCMD,"LoadLibraryExW")==0)
	{
		HookFunction("Kernel32.dll", "LoadLibraryExW", (DWORD)hook_LoadLibraryExW,bHook,&LoadLibraryExWHooker);
		return;
	}

	if (strcmp(pCMD,"ExitWindowsEx")==0)
	{
		HookFunction("User32.dll", "ExitWindowsEx", (DWORD)hook_ExitWindowsEx,bHook,&ExitWindowsHooker);
		return;
	}


	if (strcmp(pCMD,"CreateWindowExW")==0)
	{
		HookFunction("User32.dll", "CreateWindowExW", (DWORD)hook_CreateWindowExW,bHook,&CreateWindowExWHooker);
		return;
	}

	if (strcmp(pCMD,"SetWindowPos")==0)
	{
		HookFunction("User32.dll", "SetWindowPos", (DWORD)hook_SetWindowPos,bHook,&SetWindowPosHooker);
		return;
	}

	if (strcmp(pCMD,"RegQueryValueExA")==0)
	{
		HookFunction("ADVAPI32.dll", "RegQueryValueExA", (DWORD)hook_RegQueryValueExA,bHook,&RegQueryValueExAHooker);
		return;
	}

	if (strcmp(pCMD,"RegQueryValueExW")==0)
	{
		HookFunction("ADVAPI32.dll", "RegQueryValueExW", (DWORD)hook_RegQueryValueExW,bHook,&RegQueryValueExWHooker);
		return;
	}

	if (strcmp(pCMD,"RegOpenKeyExA")==0)
	{
		HookFunction("ADVAPI32.dll", "RegOpenKeyExA", (DWORD)hook_RegOpenKeyExA,bHook,&RegOpenKeyExAHooker);
		return;
	}

	if (strcmp(pCMD,"RegOpenKeyExW")==0)
	{
		HookFunction("ADVAPI32.dll", "RegOpenKeyExW", (DWORD)hook_RegOpenKeyExW,bHook,&RegOpenKeyExWHooker);
		return;
	}

	if (strcmp(pCMD,"RegOpenKeyA")==0)
	{
		HookFunction("ADVAPI32.dll", "RegOpenKeyA", (DWORD)hook_RegOpenKeyA,bHook,&RegOpenKeyAHooker);
		return;
	}

	if (strcmp(pCMD,"RegSetKeyValueA")==0)
	{
		HookFunction("ADVAPI32.dll", "RegSetKeyValueA", (DWORD)hook_RegSetKeyValueA,bHook,&RegSetKeyValueAHooker);
		return;
	}


	if (strcmp(pCMD,"RegSetValueExA")==0)
	{
		HookFunction("ADVAPI32.dll", "RegSetValueExA", (DWORD)hook_RegSetValueExA,bHook,&RegSetValueExAHooker);
		return;
	}

	if (strcmp(pCMD,"RegSetValueExW")==0)
	{
		HookFunction("ADVAPI32.dll", "RegSetValueExW", (DWORD)hook_RegSetValueExW,bHook,&RegSetValueExWHooker);
		return;
	}

	if (strcmp(pCMD,"RegCreateKeyA")==0)
	{
		HookFunction("ADVAPI32.dll", "RegCreateKeyA", (DWORD)hook_RegCreateKeyA,bHook,&RegCreateKeyAHooker);
		return;
	}

	if (strcmp(pCMD,"RegCreateKeyExA")==0)
	{
		HookFunction("ADVAPI32.dll", "RegCreateKeyExA", (DWORD)hook_RegCreateKeyExA,bHook,&RegCreateKeyExAHooker);
		return;
	}

	if (strcmp(pCMD,"RegCreateKeyExW")==0)
	{
		HookFunction("ADVAPI32.dll", "RegCreateKeyExW", (DWORD)hook_RegCreateKeyExW,bHook,&RegCreateKeyExWHooker);
		return;
	}

	if (strcmp(pCMD,"connect")==0)
	{
		HookFunction("WS2_32.dll", "connect", (DWORD)hook_connect,bHook,&connectHooker);
		return;
	}

	if (strcmp(pCMD,"recv")==0)
	{
		HookFunction("WS2_32.dll", "recv", (DWORD)hook_recv,bHook,&recvHooker);
		return;
	}

	if (strcmp(pCMD,"recvfrom")==0)
	{
		HookFunction("WS2_32.dll", "recvfrom", (DWORD)hook_recvfrom,bHook,&recvfromHooker);
		return;
	}

	if (strcmp(pCMD,"send")==0)
	{
		HookFunction("WS2_32.dll", "send", (DWORD)hook_send,bHook,&sendHooker);
		return;
	}

	if (strcmp(pCMD,"sendto")==0)
	{
		HookFunction("WS2_32.dll", "sendto", (DWORD)hook_sendto,bHook,&sendtoHooker);
		return;
	}

	if (strcmp(pCMD,"FindWindowExA")==0)
	{
		HookFunction("user32.dll", "FindWindowExA", (DWORD)hook_FindWindowExA,bHook,&FindWindowExAHooker);
		return;
	}

	if (strcmp(pCMD,"FindWindowExW")==0)
	{
		HookFunction("user32.dll", "FindWindowExW", (DWORD)hook_FindWindowExW,bHook,&FindWindowExWHooker);
		return;
	}

	if (strcmp(pCMD,"CreateFileA")==0)
	{
		HookFunction("Kernel32.dll", "CreateFileA", (DWORD)hook_CreateFileA,bHook,&CreateFileAHooker);
		return;
	}

	if (strcmp(pCMD,"CreateFileW")==0)
	{
		HookFunction("Kernel32.dll", "CreateFileW", (DWORD)hook_CreateFileW,bHook,&CreateFileWHooker);
		return;
	}

	if (strcmp(pCMD,"SetFileAttributesA")==0)
	{
		HookFunction("Kernel32.dll", "SetFileAttributesA", (DWORD)hook_SetFileAttributesA,bHook,&SetFileAttributesAHooker);
		return;
	}

	if (strcmp(pCMD,"SetFileAttributesW")==0)
	{
		HookFunction("Kernel32.dll", "SetFileAttributesW", (DWORD)hook_SetFileAttributesW,bHook,&SetFileAttributesWHooker);
		return;
	}

	if (strcmp(pCMD,"SetWindowTextA")==0)
	{
		HookFunction("User32.dll", "SetWindowTextA", (DWORD)hook_SetWindowTextA,bHook,&SetWindowTextAHooker);
		return;
	}

	if (strcmp(pCMD,"SetWindowTextW")==0)
	{
		HookFunction("User32.dll", "SetWindowTextW", (DWORD)hook_SetWindowTextW,bHook,&SetWindowTextWHooker);
		return;
	}

	if (strcmp(pCMD,"MessageBoxA")==0)
	{
		HookFunction("User32.dll", "MessageBoxA", (DWORD)hook_MessageBoxA,bHook,&MessageBoxAHooker);
		return;
	}

	if (strcmp(pCMD,"MessageBoxW")==0)
	{
		HookFunction("User32.dll", "MessageBoxW", (DWORD)hook_MessageBoxW,bHook,&MessageBoxWHooker);
		return;
	}

	if (strcmp(pCMD,"WriteFile")==0)
	{
		HookFunction("Kernel32.dll", "WriteFile", (DWORD)hook_WriteFile,bHook,&WriteFileHooker);
		return;
	}

	if (strcmp(pCMD,"DeviceIoControl")==0)
	{
		HookFunction("Kernel32.dll", "DeviceIoControl", (DWORD)hook_DeviceIoControl,bHook,&DeviceIoControlHooker);
		return;
	}

	if (strcmp(pCMD,"WriteProcessMemory")==0)
	{
		HookFunction("Kernel32.dll", "WriteProcessMemory", (DWORD)hook_WriteProcessMemory,bHook,&WriteProcessMemoryHooker);
		return;
	}

	if (strcmp(pCMD,"HTTP")==0)
	{
		HookFunction("ntdll.dll", "NtDeviceIoControlFile", (DWORD)hook_NtDeviceIoControlFile,bHook,&NtDeviceIoControlFileHooker);
		return;
	}

	PEDOLL_ACK_PACKET ackpack;
	ackpack.Return=0;
	g_PeDollNetwork.SendAckPack((char *)&ackpack,sizeof(PEDOLL_ACK_PACKET));

}

void TestCall(char *a,char *b)
{
	_asm
	{
		mov edx,[esp+4]
		mov ecx,[esp+8]
		test edx,edx
		mov eax,1
	}
	printf(a);
	printf(b);
}

 int main()
 {
 	char buffer[10*1024];
	char buffer2[10*1024];
 	memset(HookMessage,0,sizeof(HookMessage));
 
 	if(!InitializeNetwork())
 	{
 		if(MessageBoxA(NULL,"无法初始化调试通道.","启动信息",MB_YESNO|MB_ICONWARNING)==PEDOLL_EXECUTE_REJECT)
 			exit(0);
 	}
	system("pause");
	SIZE_T t;
	int it=0;
	for(int j=0;j<10;j++)
	{
		for (int i=0;i<1024;i++)
		{
		
		buffer[it]='a'+j;
		buffer2[it]='a'+j;
		it++;
		}
	}
	WriteProcessMemory(GetCurrentProcess(),buffer,buffer2,sizeof(buffer2),&t);
	while (1)
	{
		Sleep(10000);
	}
 }
