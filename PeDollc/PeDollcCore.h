#pragma once
#define _WIN32_DCOM
#include <QObject>
#include <QColor>
#include <Queue>
#include "../PainterEngineSubPacket/network/inc/Cube_TCPFastIO.h"
#include "../protocol.h"
#include "../Grammar/Cube_Grammar.h"
#include <iostream>
#include <comdef.h>
#include <Wbemidl.h>
#include <math.h>
#include <Shlobj.h>
#include <../udis86/udis86.h>

# pragma comment(lib, "wbemuuid.lib")
# pragma comment(lib, "shell32.lib")
#define  PEDOLL_DEFAULE_DUMPPACKET_MAXSIZE	8192
#define  PEDOLL_DEFAULE_DUMPPACKET_MAXCOUNT	512
#define  PEDOLL_DEFAULT_STACKCOUNT			64
using namespace std;

enum QUERY_RETURN
{
	QUERY_RETURN_PASS,
	QUERY_RETURN_REJECT,
	QUERY_RETURN_QUERY,
	QUERY_RETURN_TERMINATE
};

struct PEDOLL_QUERY_STRUCT
{
	QString APIName;
	QString msdnNote;
	QString Behavior;

	int		packetID;
	PEDOLL_QUERY_STRUCT()
	{
		APIName="";
	}
};

struct PEDOLL_DUMP_PACK
{
	QString  Note;
	int		 Size;
	pt_byte *Dump;
};

struct PEDOLL_MEMORY_INFO
{
	unsigned int address;
	int  len;
	char Buffer[PEDOLL_STACK_MEMORY_LEN];
	PEDOLL_MEMORY_INFO()
	{
		address=0;
		len=0;
		Buffer[0]='\0';
	}

};

struct PEDOLL_STACK_INFO
{
	unsigned int address;
	unsigned int dwordVal;
	char StringPointer[PEDOLL_STACK_MEMORY_LEN];
};

struct PEDOLL_MEMORYDUMP_INFO
{
	int size;
	char buffer[PEDOLL_DUMP_MEMORY_LEN];
};

class PeDollcCore;

class PEDollc_NetWork:public Cube_TCPFastIO_Client
{
public:
	PEDollc_NetWork()
	{
		m_CommandAckHandle=CreateEvent(NULL,TRUE,FALSE,NULL);
		m_Port=0;
	};
	~PEDollc_NetWork(){};

	BOOL ConnectServer(const char *host);
	void AsDoll(){m_IsDoll=true;m_IsController=false;}
	void AsController(){m_IsDoll=false;m_IsController=true;}
	void SetPort(int port){m_Port=port;}
	void SetCore(PeDollcCore *core);
	void OnReceived(IN pt_int flag, IN pt_byte *Buffer, IN int Size);
	void OnDisconnected(IN pt_int flag);
	BOOL SendCommand(char*cmd,unsigned int *ret=NULL);
	BOOL GetAtomBuffer(unsigned int address,int size);
private:
	HANDLE		 m_CommandAckHandle;
	unsigned int m_LastReturn;
	int			m_Port;
	PeDollcCore *m_pCore;
	bool		m_IsDoll;
	bool		m_IsController;
};

class PeDollcCore:public QObject
{
	Q_OBJECT
public:
	PeDollcCore(void);
	~PeDollcCore(void);
	BOOL Initialize();
	BOOL StartupNetwork();
	BOOL IsReg();
	BOOL IsRegMark(){return m_IsRegistied;}
	void ShutDownNetWork();
	void Free();
	DWORD GetSerialDWORD();

	void OnDollDisconnect();;
	void OnControllerDisconnect();;
	void OnDollReceive(pt_byte *Buffer, IN int Size);;
	void OnControllerReceive(pt_byte *Buffer, IN int Size);;

	void ExecuteCommand(char *Command);
	
	BOOL ExecuteControllerCommand(char *Command);
	BOOL ExecuteDollCommand(char *Command);

	void				addQuery(PEDOLL_QUERY_STRUCT);
	void				popQuery();
	void				clearQueries();
	void				SetupHost(QString);
	void				ParseDollQuery(PEDOLL_HOOKINFO_PACKET Query);
	void				InitializeCommandGrammar();
	void				InitializeQueryGrammar();
	void				RemoveDumpPack(unsigned int index);

	void				ClearDumpPack();
	void				AddFunctionFilter(string);
	void				ClearFunctionFilter();
	
	BOOL				ReadStackMemory(DWORD address,int size);
	int					ReadAtomMemory(char buffer[PEDOLL_MEMORY_LEN],DWORD address,int size);
	BOOL				ReadMemory(DWORD address,int size);
	void				SetDumpMaxSize(int Size){m_MaxDumpSize=Size;}
	void				SetDumpMaxCount(int Count){m_MaxDumpCout=Count;}
	int					GetMaxDumpSize(){return m_MaxDumpSize;}
	int					GetMaxDumpCount(){return m_MaxDumpCout;}
	bool				QueryParamMatch(char *strQueryParam,char *Match);
	char				*GetProcName(){return m_ProcName;}
	BOOL				UpdateStack(unsigned int dfesp=0);
	unsigned int		GetUpdateEsp(){return m_UpdateESP;};
	BOOL									ConnectToController();
	BOOL									ConnectToDoll();

	
	QUERY_RETURN							SenQuery(CubeGrammarSentence &Sen,int MatchParamCount,bool &bShow);
	PEDOLL_QUERY_STRUCT						getQuery();
	vector<int>								FindFilterQueryString(char *strQueryHeader);
	vector<PEDOLL_DUMP_PACK>				GetDumpPacketsSnapshot(){return m_DumpPacket;}
	vector<PEDOLL_STACK_INFO>				GetStackInfoVector(){return m_StackInfo;}
	char *									GetStackBuffer(){return (char *)m_dwordstack;}
	PEDollc_NetWork							*GetDollNetWorkInterface(){return &m_DollNetWork;}
	PEDollc_NetWork							*GetControllerNetWorkInterface(){return &m_ControllerNetWork;}
	vector<PEDOLL_MEMORYDUMP_INFO> m_memSerial;
signals:
	void SIGNAL_EmitMonitorMessage(QString,QColor);
	void SIGNAL_EmitRegistryMessage(QString,QColor);
	void SIGNAL_EmitFilesMessage(QString,QColor);
	void SIGNAL_EmitProcessesMessage(QString,QColor);
	void SIGNAL_EmitWindowsOperatorMessage(QString,QColor);
	void SIGNAL_EmitNetworkMessage(QString,QColor);
	void SIGNAL_EmitDumpTableUpdate();
	void SIGNAL_ConnectionStatisChanged();
	void SIGNAL_onQuery();

	
private:
	CRITICAL_SECTION	m_QueryCS,m_DumpCS;  
	PEDollc_NetWork m_DollNetWork;
	PEDollc_NetWork m_ControllerNetWork;
	PEDOLL_MEMORY_INFO	m_LastReturnBuffer;
	PEDOLL_MEMORY_INFO	m_CurrentViewBuffer;
	vector<PEDOLL_STACK_INFO> m_StackInfo;
	unsigned int		m_lastQueryESP;
	unsigned int		m_UpdateESP;
	QString				m_Host;
	CubeGrammar			m_CommandGrammar,m_QueryGrammar;
	CubeLexer			m_CommandLexer,m_QueryLexer,m_FilterLexer;
	char				m_ProcName[MAX_PATH];
	int					m_instr_doll,m_instr_shell,m_instr_EnumProcess,m_instr_ls,m_Instr_hook,m_Instr_unhook,m_instr_kill,m_Instr_binary,m_Instr_unbinary;
	DWORD				m_dwordstack[PEDOLL_DEFAULT_STACKCOUNT];
	vector<string>		m_FunctionFilter;
	vector<PEDOLL_DUMP_PACK>    m_DumpPacket;
	queue<PEDOLL_QUERY_STRUCT>	m_QueryQueue;
	BOOL				m_IsRegistied;
	int					m_MaxDumpSize;
	int					m_MaxDumpCout;
	int					m_Reply_REGOPENKEYEXA;
	int					m_Reply_REGSETKEYVALUEA;
	int					m_Reply_REGSETVALUEEXW;
	int					m_Reply_REGSETVALUEEXA;
	int					m_Reply_REGSETVALUEA;
	int					m_Reply_REGCREATEKEYA;
	int					m_Reply_FINDWINDOWEXA;
	int					m_Reply_FINDWINDOWEXW;
	int					m_Reply_REGCREATEKEYEXW;
	int					m_Reply_REGCREATEKEYEXA;
	int					m_Reply_CREATEFILEA;
	int					m_Reply_CREATEFILEW;
	int					m_Reply_CREATEPROCESSINTERNALW;
	int					m_Reply_CREATEPROCESSW;
	int					m_Reply_CREATEPROCESSA;
	int					m_Reply_CONNECT;
	int					m_Reply_SEND;
	int					m_Reply_SENDTO;
	int					m_Reply_RECV;
	int					m_Reply_RECVFROM;
	int					m_Reply_DELETEFILEA;
	int					m_Reply_DELETEFILEW;
	int					m_Reply_EXITWINDOWSEX;
	int					m_Reply_TERMINATEPROCESS;
	int					m_Reply_PROCESS32FIRSTW;
	int					m_Reply_PROCESS32NEXTW;
	int					m_Reply_OPENPROCESS;
	int					m_Reply_LOADLIBRARYEXW;
	int					m_Reply_CREATEWINDOWEXW;
	int					m_Reply_SETWINDOWPOS;
	int					m_Reply_SETFILEATTRIBUTESA;
	int					m_Reply_SETFILEATTRIBUTESW;
	int					m_Reply_SETWINDOWTEXTA;
	int					m_Reply_SETWINDOWTEXTW;
	int					m_Reply_MESSAGEBOXA;
	int					m_Reply_MESSAGEBOXW;
	int					m_Reply_WRITEFILE;
	int					m_Reply_DEVICEIOCONTROL;
	int					m_Reply_WRITEPROCESSMEMORY;
};


