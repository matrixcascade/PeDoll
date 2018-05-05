#pragma once;
#include "../PainterEngineSubPacket/network/inc/Cube_TCPFastIO.h"
#include <windows.h>
#include <stdio.h>
#pragma comment(lib,"ws2_32.lib")
#include "../PeStructure/inc/PEStructure.h" 
#include "../protocol.h"
#include "../Grammar/Cube_Grammar.h"

#include <vector>
#include <atlconv.h>
#include <tlhelp32.h>
#include <Winternl.h>
#include <queue>

#ifdef DLL_IMPLEMENT
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif


extern "C" {
	void DLL_API X_dummy();
}

#define  REMOTESHELL_PROCESS_GUID ("14B599DD-E9D3-4301-BF5D-4228B154776")


struct SEND_PACKET_INFO
{
	void *buffer;
	unsigned int Size;
	SEND_PACKET_INFO()
	{
		buffer=NULL;
		Size=0;
	}
	void free()
	{
		if (buffer)
		{
			delete [] buffer;
			Size=0;
			buffer = NULL;
		}
	}
};


class PEDoll_NetWork:public Cube_TCPFastIO_Server
{
public:
	PEDoll_NetWork();
	~PEDoll_NetWork(){}

	void OnConnected(IN SOCKET socket,IN SOCKADDR_IN	addrin,OUT pt_int &Flag);;
	void SendAckPack(char *buf,int Size);

	bool SocketSend(char *buf,int Size);

	void WaitForConnect();
	void Preload();

	unsigned int ParserCommand(char *Command);

	void OnReceived(IN pt_int flag, IN pt_byte *Buffer, IN int Size);;


	void SendDumpPack(char *Info,char *buffer,int Len);

	int GetFreeCmdID();

	BOOL QueryCommandPack(unsigned int PacketID,PEDOLL_API_ACK &pack);

	void OnDisconnected(IN pt_int flag);
	void SendControllerMessage(char *message);

	void SendMessagePack(PEDOLL_MESSAGE_PACKET  pack);

	void SendHookInfoPack(PEDOLL_HOOKINFO_PACKET pack);


	BOOL				m_Isconnect;
	SOCKET              m_cursocket;
	unsigned int		m_stID;
	HANDLE				m_NetWorkConnectEvent;
	CubeGrammar			m_Grammar;
	CubeLexer			m_Lexer;
	std::vector<PEDOLL_API_ACK> m_vAPIExec;
	CubeCriticalSection m_APIQueryCS;
	int					m_Instr_Hook,m_Instr_Unhook,m_Instr_Terminate,m_Instr_Run,m_Instr_esp,m_Instr_readMemory,m_Instr_binary,m_Instr_unbinary;
};

class  PEDoll_SendStackThread:public Cube_Thread
{
public:
	PEDoll_SendStackThread();
	void OnAck();
	void PushSendPacket(void *buffer,unsigned int Len);
	void ThreadExit();
	void PerloadExecute();
	void run();
protected:
private:
	HANDLE							m_ackPackEvent;
	HANDLE							m_SendEvent;
	bool							m_bRun;
	std::queue<SEND_PACKET_INFO>	m_PacketQueue;
	CubeCriticalSection				m_QueueCS;
};
